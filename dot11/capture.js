/* jshint node: true */

/**
 * Wi-Fi packet capturing streams.
 *
 * There are currently two types of capture streams available:
 *
 * + Live capture from a network interface.
 * + Replay capture from a PCAP saved file.
 *
 */
(function (root) {
  'use strict';

  var util = require('util'),
      binding = require('../build/Release/dot11'),
      stream = require('stream');

  /**
   * Base stream capture class.
   *
   * This class can't be instantiated directly as it isn't exported from this
   * module. It serves to provide generic methods usable by both live and
   * replay capture streams.
   *
   * @param pcap An activated PCAP handle.
   * @param buffer The buffer used to instantiate the above PCAP handle.
   * @param opts The following options:
   *
   *             + batchSize: Number of packets read during each call to PCAP.
   *               A higher number here is more efficient but runs the risk of
   *               overflowing internal buffers (especially in the case of
   *               replay streams, which can't rely on the PCAP dispatch call
   *               returning in time). [default: 1000]
   *
   */
  function Capture(pcap, buffer, opts) {

    opts = opts || {};
    var batchSize = opts.batchSize || 1000;

    stream.Readable.call(this, {objectMode: true, highWaterMark: 1});
    // The high watermark parameter doesn't make as much sense when run on
    // object mode streams. 1 is sufficient to have each read trigger the next
    // push.

    var closed = false;
    var offset = 0;
    var offsets = [0];

    this.on('end', function () {

      closed = true; // In case this wasn't caused by a call to `close`.
      pcap.close(); // Free PCAP resources.

    });

    // "Public" methods (they are not attached to the prototype so as not to
    // have to expose the PCAP handle).

    /**
    * Get underlying data link type.
    *
    * E.g. IEEE802_11_RADIO.
    *
    */
    this.getDatalink = function () { return pcap.getDatalink(); };

    /**
    * Terminate the stream.
    *
    * Note that depending on when this is called, a few more packets might be
    * emitted before the stream actually closes (this number is guaranteed to
    * be lower than the capture's batch size).
    *
    */
    this.close = function () {

      // We do not try to break the loop early here as we are guaranteed that
      // this method can only ever be called outside of a dispatch call (since
      // we don't return to the event loop until the end and we never call it
      // ourselves internally).
      closed = true;
      this.push(null);

    };

    /**
     * Check if the capture is still running.
     *
     */
    this.isClosed = function () { return closed; };

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
          pcap.dispatch(batchSize, packetHandler);
          // When reading a file, we can't rely on the output value of the
          // dispatch call so we rely on our offsets array instead.
          var nPackets = offsets.length - 1;
          self.emit(
            'fetch',
            nPackets / batchSize,
            // Fraction of batch size used. In live mode, this should be
            // consistently under 1 in order to avoid filling up PCAP's buffer
            // and losing packets there.
            offsets[nPackets] / buffer.length
            // Fraction of the buffer used. This is mostly useful in replay
            // mode, as in live mode we are guaranteed (if the buffer sizes
            // between here and PCAP as equal) that this will never overflow.
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
          // than the buffer size. Packet overflow could simply emit a warning
          // event.
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
   * @param dev The network interface to capture from.
   * @param opts Capture options. On top of the options from the `Capture`
   *             class, the following keys are supported:
   *
   *             + monitor: Capture in monitor mode. [default: false]
   *             + promisc: Capture in promiscuous mode. [default: false]
   *             + filter: BPF packet filter. [default: '']
   *             + snapLen: Snapshot length (i.e. how much of each packet is
   *               retained). [default: 65535]
   *             + bufferSize: Size of temporary buffer used by PCAP to hold
   *               packets. Larger means more packets can be gathered in fewer
   *               dispatch calls (this will effectively cap the batchSize
   *               option). [default: 1MB]
   *
   *  Note that a live capture stream will not terminate on its own. This means
   *  in particular that the node process will run until the stream is manually
   *  closed.
   *
   */
  function Live(dev, opts) {

    opts = opts || {};
    var monitor = opts.monitor || false;
    var promisc = opts.promisc || false;
    var filter = opts.filter || '';
    var snapLen = opts.snapLen || 65535; // 65kB
    var bufferSize = opts.bufferSize || 1024 * 1024; // 1 MB

    var buffer = new Buffer(bufferSize);
    var pcap = new binding.Pcap(buffer)
      .fromDevice(dev)
      .setRfMon(monitor)
      .setPromisc(promisc)
      .setFilter(filter)
      .setSnapLen(snapLen)
      .setBufferSize(bufferSize)
      .activate();

    Capture.call(this, pcap, buffer, opts);

    // Live capture specific methods.

    /**
    * Get PCAP statistics for stream.
    *
    * This includes total packets received, dropped because of buffer overflow,
    * and dropped by the interface (e.g. because of filters). See `man
    * pcap_stats` for more details. Finally, note that these statistics are not
    * exact and can even mean different things depending on the platform.
    *
    */
    this.getStats = function () { return pcap.stats(); };

    // TODO: Implement inject.

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
   * @param path Path to a saved .pcap file.
   * @param opts Capture options. On top of the options from the `Capture`
   *             class, the following option is supported:
   *
   *             + bufferSize: Size of temporary buffer used by PCAP to hold
   *               packets. Larger means more packets can be gathered in fewer
   *               dispatch calls. This option should be carefully chosen as a
   *               function of the batch size (there are no safeguard against
   *               overflows here as PCAP won't cap the amount of data returned
   *               by a dispatch call in offline mode). [default: 1MB]
   *
   * Note that unlike the live capture stream, this stream will automatically
   * close once the end of the file read is reached.
   *
   */
  function Replay(path, opts) {

    opts = opts || {};
    var bufferSize = opts.bufferSize || 1024 * 1024; // 1 MB

    var buffer = new Buffer(bufferSize);
    var pcap = new binding.Pcap(buffer).fromSavefile(path);

    Capture.call(this, pcap, buffer, opts);

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

  // Export things.

  root.exports = {
    Live: Live,
    Replay: Replay
  };

})(module);
