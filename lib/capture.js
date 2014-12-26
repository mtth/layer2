/* jshint node: true */

/**
 * Packet streams.
 *
 */
(function (root) {
  'use strict';

  var util = require('util'),
      pcap = require('../build/Release/pcap'),
      stream = require('stream');

  /**
   * Base stream capture class.
   *
   * This class isn't meant to be instantiated directly as it isn't exported
   * from this module. It serves to provide generic methods usable by both live
   * and replay capture streams.
   *
   */
  function Capture(wrapper, buffer, opts) {

    opts = opts || {};
    var batchSize = opts.batchSize || 1000;

    if (wrapper.getMaxPacketSize() > buffer.length) {
      throw new Error('Buffer size should be greater than snapshot length.');
      // Otherwise the internal buffer might not fit even a single packet.
    }

    stream.Readable.call(this, {objectMode: true, highWaterMark: 1});
    // The high watermark parameter doesn't make as much sense when run on
    // object mode streams. 1 is sufficient to have each read trigger the next
    // push.

    var closed = false;
    var offset = 0;
    var offsets = [0];

    this.on('end', function () {

      closed = true; // In case this wasn't caused by a call to `close`.
      wrapper.close(); // Free PCAP resources.

    });

    // "Public" methods (they are not attached to the prototype so as not to
    // have to expose the PCAP handle).

    this.getLinkType = function () { return wrapper.getLinkType(); };

    this.getMaxPacketSize = function () { return wrapper.getMaxPacketSize(); };

    this.close = function () {

      // We do not try to break the loop early here as we are guaranteed that
      // this method can only ever be called outside of a dispatch call (since
      // we don't return to the event loop until the end and we never call it
      // ourselves internally).
      closed = true;
      this.push(null);
      this.emit('close');

    };

    // "Private" methods (to be used by subclasses). Similarly to above we
    // don't attach them to the prototype (which actually also lets us have a
    // small performance gain by speeding up method calls).

    this._hasPacket = function () { return offset + 1 < offsets.length; };

    this._getPacket = function () {

      var prevOffset = offset++;
      return buffer.slice(offsets[prevOffset], offsets[offset]);

    };

    this._fetchPackets = function (callback) {
      // Fetches are relatively expensive (and slow, one per tick), we
      // therefore want to make sure they happen as little as possible.

      var self = this;

      setImmediate(function () {
        // We wrap the dispatch call in a `setImmediate` in order to not defer
        // back to the event loop. Ideally, the packet handlers would be called
        // asynchronously however the current implementation has the
        // `_dispatch` call block until all the packets from this batch have
        // been processed. Furthermore, using `process.nextTick` doesn't seem
        // to be sufficient to let other code run. Finally, this makes tuning
        // the batch size and buffer sizes very relevant to performance.

        if (self._hasPacket()) {
          self.emit('error', new Error('Preemptive fetch.'));
        }

        if (closed) {
          // The capture might have been closed in the meantime, only dispatch
          // the fetch request if the capture is still open.
          self.emit('fetch');
        } else {
          offset = 0;
          offsets = [0];
          wrapper.dispatch(batchSize, packetHandler);
          // When reading a file, we can't rely on the output value of the
          // dispatch call so we rely on our offsets array instead.
          var nPackets = offsets.length - 1;
          self.emit(
            'fetch',
            nPackets / batchSize,
            offsets[nPackets] / buffer.length
          );
          callback.call(self, nPackets);
        }

      });

      function packetHandler(packetOffset, bufOverflow, packetOverflow) {

        if (bufOverflow || packetOverflow) {
          // TODO: use break loop (and a side buffer) to prevent buffer
          // overflows from happening. Note that is only relevant for the
          // replay use-case (as long as the live capture's buffer is the same
          // length as PCAP's buffer, we are guaranteed that this won't
          // happen). Note that to have this work correctly in all cases, we
          // probably then also must require the snapshot length to be smaller
          // than the buffer size.
          self.emit('error', new Error('Buffer overflow.'));
        }
        offsets.push(packetOffset);

      }

    };

  }
  util.inherits(Capture, stream.Readable);

  /**
   * Live packet capture stream.
   *
   */
  function Live(dev, opts) {

    opts = opts || {};
    var monitor = opts.monitor || false;
    var promisc = opts.promisc || false;
    var filter = opts.filter || '';
    var maxPacketSize = opts.maxPacketSize || 65535; // 65kB
    var bufferSize = opts.bufferSize || 1024 * 1024; // 1 MB

    var buffer = new Buffer(bufferSize);
    var wrapper = new pcap.Wrapper(buffer)
      .fromDevice(dev)
      .setMonitor(monitor)
      .setPromisc(promisc)
      .setFilter(filter)
      .setMaxPacketSize(maxPacketSize)
      .setBufferSize(bufferSize)
      .activate();

    Capture.call(this, wrapper, buffer, opts);

    this.getStats = function () { return wrapper.getStats(); };

    // TODO: Implement inject (as a writable part of this stream or as a new
    // writable stream?).

  }
  util.inherits(Live, Capture);

  Live.prototype._read = function () {

    if (!this._hasPacket()) {
      this._fetchPackets(function (nPackets) {
        if (nPackets) {
          this.push(this._getPacket());
        } else {
          // Try again later.
          this._read.bind(this);
        }
      });
    } else {
      this.push(this._getPacket());
    }

  };

  // TODO: Implement getDefaultInterface.

  /**
   * Packet capture stream from saved file.
   *
   */
  function Replay(path, opts) {

    opts = opts || {};
    var bufferSize = opts.bufferSize || 1024 * 1024; // 1 MB

    var buffer = new Buffer(bufferSize);
    var wrapper = new pcap.Wrapper(buffer).fromSavefile(path);

    Capture.call(this, wrapper, buffer, opts);

  }
  util.inherits(Replay, Capture);

  Replay.prototype._read = function () {

    if (!this._hasPacket()) {
      this._fetchPackets(function (nPackets) {
        if (nPackets) {
          this.push(this._getPacket());
        } else {
          // We've reached EOF.
          this.close();
        }
      });
    } else {
      this.push(this._getPacket());
    }

  };

  /**
   * Save capture to file.
   *
   */
  function Save(path, opts) {

    opts = opts || {};
    var linkType = opts.linkType || null; // Inferred from pipe.
    var maxPacketSize = opts.maxPacketSize || 65535;

    stream.Writable.call(this, {objectMode: true});

    var wrapper = null;

    this.on('pipe', function (src) {
      // Infer link type from first capture stream piped.

      if (linkType === null) {
        linkType = src.getLinkType();
      } else if (linkType !== src.getLinkType()) {
        throw new Error('Inconsistent link type.');
      }

    });

    this.on('finish', function () { this.close(); });
    // Close (and flush to) the underlying file when the stream ends (this can
    // be triggered by calling the built-in `end` stream method (also called
    // automatically when calling `pipe`, unless disabled via options).

    this.close = function () { wrapper.close(); this.emit('close'); };

    this.getLinkType = function (linkType) { return linkType; };

    this._write = function (chunk, encoding, callback) {

      if (!wrapper) {
        if (linkType === null) {
          throw new Error('No link type specified.');
        }
        wrapper = new pcap.Wrapper(new Buffer(0)) // Unused buffer.
          .fromDead(linkType, maxPacketSize)
          .toSavefile(path);
      }

      wrapper.dumpPacket(chunk);
      callback();

    };

  }
  util.inherits(Save, stream.Writable);

  // Export things.

  root.exports = {
    Live: Live,
    Replay: Replay,
    Save: Save
  };

})(module);
