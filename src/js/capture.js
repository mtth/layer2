/* jshint node: true */

/**
 * Frame capture and injection streams.
 *
 */
(function (root) {
  'use strict';

  var util = require('util'),
      stream = require('stream'),
      addon = require('./utils').requireAddon();

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
    var maxFrameSize = wrapper.getMaxFrameSize();

    if (maxFrameSize > buffer.length) {
      throw new Error('Buffer size should be greater than max frame size.');
      // Otherwise the internal buffer might not fit even a single frame and
      // we wouldn't be guaranteed to not lose any frames, even if we break
      // the dispatch loop.
    }

    // "Public" methods (they are not attached to the prototype so as not to
    // have to expose the PCAP wrapper).

    this.getLinkType = function () { return wrapper.getLinkType(); };

    this.getMaxFrameSize = function () { return wrapper.getMaxFrameSize(); };

    this.close = function (timeout) {

      var self = this;
      setTimeout(function () { self.push(null); }, timeout || 0);
      return this;

    };

    // "Private" methods (to be used by subclasses). Similarly to above we
    // don't attach them to the prototype (which actually also lets us have a
    // small performance gain by speeding up method calls).

    this._hasFrame = function () { return offset + 1 < offsets.length; };

    this._getFrame = function () {

      var prevOffset = offset++;
      return buffer.slice(offsets[prevOffset], offsets[offset]);

    };

    this._sendFrame = function (buf) {

      if (closed) {
        return this.emit('error', new Error('Cannot send after close.'));
      }
      wrapper.injectFrame(buf);

    };

    this._fetchFrames = function (callback) {
      // Fetches are relatively expensive (and slow, one per tick), we
      // therefore want to make sure they happen as little as possible.

      var self = this;

      setImmediate(function () {
        // We wrap the dispatch call in a `setImmediate` in order to defer back
        // to the event loop once in a while. Note that this makes tuning the
        // batch size and buffer sizes very relevant to performance.

        if (self._hasFrame()) {
          return self.emit('error', new Error('Preemptive fetch.'));
        }

        if (closed) {
          // The capture might have been closed in the meantime, only dispatch
          // the fetch request if the capture is still open.
          self.emit('fetch');
        } else {
          offset = 0;
          offsets = [0];
          if (wrapper.dispatch(batchSize, frameHandler) === -2) {
            // Loop was broken, needed to clear flag.
            wrapper.dispatch(batchSize, frameHandler);
          }
          // When reading a file, we can't rely on the output value of the
          // dispatch call so we rely on our offsets array instead.
          var nFrames = offsets.length - 1;
          self.emit(
            'fetch',
            nFrames / batchSize,
            offsets[nFrames] / buffer.length
          );
          callback.call(self, nFrames);
        }

      });

      function frameHandler(frameOffset, bufOverflow, frameOverflow) {

        if (frameOverflow) {
          self.emit('error', new Error('Frame truncation.'));
          // Don't return though, let frame through in case this is caught.
        }
        if (bufOverflow > - maxFrameSize) {
          if (bufOverflow <= 0) { // Safety.
            wrapper.breakLoop();
            self.emit('break');
          } else { // Too late (this should never happen).
            return self.emit('error', new Error('Buffer overflow.'));
          }
        }
        offsets.push(frameOffset);

      }

    };

    this._closeWrapper = function () {

      closed = true;
      wrapper.close(); // Free PCAP resources.
      this.emit('close');

    };

  }

  /**
   * Live frame capture stream.
   *
   */
  function Live(dev, opts) {

    dev = dev || Live.getDefaultDevice();
    opts = opts || {};
    var monitor = opts.monitor || false;
    var promisc = opts.promisc || false;
    var filter = opts.filter || '';
    var maxFrameSize = opts.maxFrameSize || 65535; // 65kB
    var bufferSize = opts.bufferSize || 1024 * 1024; // 1 MB

    var ended = false;
    var finished = false;
    var buffer = new Buffer(bufferSize);
    var wrapper = new addon.PcapWrapper(buffer)
      .fromDevice(dev)
      .setMonitor(monitor)
      .setPromisc(promisc)
      .setFilter(filter)
      .setMaxFrameSize(maxFrameSize)
      .setBufferSize(bufferSize)
      .activate();

    stream.Duplex.call(this, {
      allowHalfOpen: true, // But actually false (see below).
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

    this.once('end', function () {

      ended = true;
      process.nextTick(function () {
        if (!finished) {
          self.end();
        } else {
          self._closeWrapper();
        }
      });

    });

    this.once('finish', function () {

      finished = true;
      process.nextTick(function () {
        if (!ended) {
          self.push(null);
        } else {
          self._closeWrapper();
        }
      });

    });

    this.getDevice = function () { return dev; };

    this.getStats = function () { return wrapper.getStats(); };

  }
  util.inherits(Live, stream.Duplex);

  Live.prototype._read = function () {

    if (!this._hasFrame()) {
      this._fetchFrames(function (nFrames) {
        if (nFrames) {
          this.push(this._getFrame());
        } else {
          // Try again later.
          setImmediate(this._read.bind(this));
        }
      });
    } else {
      this.push(this._getFrame());
    }

  };

  Live.prototype._write = function (data, encoding, callback) {

    this._sendFrame(data);
    callback();

  };

  Live.getDefaultDevice = function () {

    return addon.getDefaultDevice();

  };

  /**
   * Frame capture stream from saved file.
   *
   */
  function Replay(fpath, opts) {

    opts = opts || {};
    var bufferSize = opts.bufferSize || 1024 * 1024; // 1 MB

    var buffer = new Buffer(bufferSize);
    var wrapper = new addon.PcapWrapper(buffer).fromSavefile(fpath);

    stream.Readable.call(this, {objectMode: true, highWaterMark: 1});

    Capture.call(this, wrapper, buffer, opts);

    this.once('end', function () {

      process.nextTick(this._closeWrapper.bind(this));

    });

    this.getPath = function () { return fpath; };

  }
  util.inherits(Replay, stream.Readable);

  Replay.prototype._read = function () {

    if (!this._hasFrame()) {
      this._fetchFrames(function (nFrames) {
        if (nFrames) {
          this.push(this._getFrame());
        } else {
          // We've reached EOF.
          this.close();
        }
      });
    } else {
      this.push(this._getFrame());
    }

  };

  /**
   * Save capture to file.
   *
   */
  function Save(fpath, opts) {

    opts = opts || {};
    var linkType = opts.linkType || null; // Inferred from pipe.
    var maxFrameSize = opts.maxFrameSize || 65535;

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

    this.once('finish', function () {
      // Close (and flush to) the underlying file when the stream ends (this can
      // be triggered by calling the built-in `end` stream method (also called
      // automatically when calling `pipe`, unless disabled via options).

      var self = this;

      process.nextTick(function () {
        if (wrapper) {
          wrapper.close();
        }
        self.emit('close');
      });

    });

    this.getPath = function () { return fpath; };

    this.getLinkType = function () { return linkType; };

    this.getMaxFrameSize = function () { return maxFrameSize; };

    this._write = function (data, encoding, callback) {

      if (!wrapper) {
        if (linkType === null) {
          return callback(new Error('No link type specified.'));
        }
        wrapper = new addon.PcapWrapper(new Buffer(0)) // Unused buffer.
          .fromDead(linkType, maxFrameSize)
          .toSavefile(fpath);
      }
      wrapper.dumpFrame(data);
      return callback();

    };

  }
  util.inherits(Save, stream.Writable);

  // Helpers.

  function summarize(fpath, cb) {

    var nFrames = 0;
    var nBytes = 0;

    new Replay(fpath)
      .on('data', function (buf) {
        nFrames++;
        nBytes += buf.length;
      })
      .once('end', function () {
        var summary = {
          linkType: this.getLinkType(),
          maxFrameSize: this.getMaxFrameSize(),
          nFrames: nFrames,
          nBytes: nBytes
        };
        cb(null, summary);
      });

  }

  // Export things.

  root.exports = {
    Live: Live,
    Replay: Replay,
    Save: Save,
    summarize: summarize
  };

})(module);
