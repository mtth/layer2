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
   * Base capture class.
   *
   * This class isn't meant to be instantiated directly as it isn't exported
   * from this module. It serves to provide generic methods usable by both live
   * and replay capture streams.
   *
   * It also isn't a stream directly (as live is readable/writable and replay
   * is readable).
   *
   */
  function Capture(wrapper, buffer, opts) {

    opts = opts || {};
    var batchSize = opts.batchSize || 1000;

    var closed = false;
    var offset = 0;
    var offsets = [0];
    var maxPacketSize = wrapper.getMaxPacketSize();

    if (maxPacketSize > buffer.length) {
      throw new Error('Buffer size should be greater than max packet size.');
      // Otherwise the internal buffer might not fit even a single packet and
      // we wouldn't be guaranteed to not lose any packets, even if we break
      // the dispatch loop.
    }

    // "Public" methods (they are not attached to the prototype so as not to
    // have to expose the PCAP wrapper).

    this.getLinkType = function () { return wrapper.getLinkType(); };

    this.getMaxPacketSize = function () { return wrapper.getMaxPacketSize(); };

    this.close = function (timeout) {
      // We do not try to break the loop early here as we are guaranteed that
      // this method can only ever be called outside of a dispatch call (since
      // we don't return to the event loop until the end and we never call it
      // ourselves internally).

      var self = this;
      setTimeout(function () { self.push(null); }, timeout || 0);
      return this;

    };

    // "Private" methods (to be used by subclasses). Similarly to above we
    // don't attach them to the prototype (which actually also lets us have a
    // small performance gain by speeding up method calls).

    this._hasPacket = function () { return offset + 1 < offsets.length; };

    this._getPacket = function () {

      var prevOffset = offset++;
      return buffer.slice(offsets[prevOffset], offsets[offset]);

    };

    this._sendPacket = function (buf) {

      if (closed) {
        this.emit('error', new Error('Cannot send after close.'));
      }
      wrapper.injectPacket(buf);

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
          if (wrapper.dispatch(batchSize, packetHandler) === -2) {
            // Loop was broken, needed to clear flag.
            wrapper.dispatch(batchSize, packetHandler);
          }
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

        if (packetOverflow) {
          self.emit('error', new Error('Packet truncation.'));
        }
        if (bufOverflow > - maxPacketSize) {
          if (bufOverflow <= 0) { // Safety.
            wrapper.breakLoop();
            self.emit('break');
          } else { // Too late (this should never happen).
            self.emit('error', new Error('Buffer overflow.'));
          }
        }
        offsets.push(packetOffset);

      }

    };

    this._closeWrapper = function () {

      closed = true;
      wrapper.close(); // Free PCAP resources.
      this.emit('close');

    };

  }

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

    var ended = false;
    var finished = false;
    var buffer = new Buffer(bufferSize);
    var wrapper = new pcap.Wrapper(buffer)
      .fromDevice(dev)
      .setMonitor(monitor)
      .setPromisc(promisc)
      .setFilter(filter)
      .setMaxPacketSize(maxPacketSize)
      .setBufferSize(bufferSize)
      .activate();

    stream.Duplex.call(this, {
      allowHalfOpen: true, // But actually true (see below).
      objectMode: true,
      highWaterMark: 1
    });

    Capture.call(this, wrapper, buffer, opts);

    // The following two methods are our own implementation of the duplex's
    // `allowHalfOpen` option. Indeed, it seems that the default implementation
    // doesn't work properly (`end()` doesn't trigger `'end'`). Although there
    // are probably simpler ways of doing this, this implementation has the
    // added advantage that the `'close'` event is guaranteed to occur after
    // both the `'end'` and `'finish'` events.

    var self = this;

    this.on('end', function () {

      ended = true;
      process.nextTick(function () {
        if (!finished) {
          self.end();
        } else {
          self._closeWrapper();
        }
      });

    });

    this.on('finish', function () {

      finished = true;
      process.nextTick(function () {
        if (!ended) {
          self.push(null);
        } else {
          self._closeWrapper();
        }
      });

    });

    this.getStats = function () { return wrapper.getStats(); };

  }
  util.inherits(Live, stream.Duplex);

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

  Live.prototype._write = function (chunk, encoding, callback) {

    this._sendPacket(chunk);
    callback();

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

    stream.Readable.call(this, {
      objectMode: true,
      highWaterMark: 1
    });

    Capture.call(this, wrapper, buffer, opts);

    this.on('end', this._closeWrapper.bind(this));

  }
  util.inherits(Replay, stream.Readable);

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
        this.emit('error', new Error('Inconsistent link type.'));
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
          this.emit('error', new Error('No link type specified.'));
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
