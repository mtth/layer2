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
  function Capture(dispatcher, opts, fn) {

    opts = opts || {};
    var batchHighWaterMark = opts.batchHighWaterMark || 2;
    var batchSize = opts.batchSize || 500;

    var self = this;
    var concurrency = 1;
    var iters = [];
    var needPush = false;
    var finishing = false;
    var cb = fn(batchCb);

    this.getLinkType = function () { return dispatcher.getDatalink(); };

    this.getMaxFrameSize = function () { return dispatcher.getSnaplen(); };

    this._read = function () {

      var frame = null;
      while (iters.length && (frame = iters[0].next()) === null) {
        // Consume all iterators until we find something, or run out.
        iters.shift();
      }

      if (!iters.length && !finishing) {
        // `frame` must be null too. We wait for the next batch to arrive.
        needPush = true;
      } else {
        // `frame` is null if finishing and not otherwise. Push it either way!
        this.push(frame);
      }

      if (concurrency > 0 && !finishing && iters.length < batchHighWaterMark) {
        concurrency--;
        dispatcher.dispatch(batchSize, cb);
      }

    };

    function batchCb(err, iter, nFrames) {

      concurrency++;
      if (err) {
        self.emit('error', err);
        // Don't return though.
      }
      if (!iter) {
        finishing = true;
      } else {
        self.emit('batch', nFrames);
        iters.push(iter);
      }
      if (needPush) {
        needPush = false;
        self._read();
      }

    }

  }

  /**
   * Frame capture stream from saved file.
   *
   */
  function Replay(fpath, opts) {

    opts = opts || {};
    var filter = opts.filter || '';

    stream.Readable.call(this, {objectMode: true, highWaterMark: 1});
    var dispatcher = addon.Dispatcher
      .fromSavefile(fpath)
      .setFilter(filter);

    Capture.call(this, dispatcher, opts, function (cb) {
      return function (err, iter, nFrames) {
        if (nFrames > 0) {
          cb(err, iter, nFrames);
        } else {
          cb(err, null);
        }
      };
    });

    this.getPath = function () { return fpath; };

  }
  util.inherits(Replay, stream.Readable);

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
        wrapper = new addon.PcapWrapper()
          .fromDead(linkType, maxFrameSize)
          .toSavefile(fpath);
      }
      wrapper.dumpFrame(data);
      return callback();

    };

  }
  util.inherits(Save, stream.Writable);

  /**
   * Live frame capture stream.
   *
   */
  function Live(dev, opts) {

    opts = opts || {};
    var monitor = opts.monitor || false;
    var promisc = opts.promisc || false;
    var filter = opts.filter || '';
    var timeout = opts.timeout || 100;
    var maxFrameSize = opts.maxFrameSize || 65535; // 65kB
    var bufferSize = opts.bufferSize || 1024 * 1024; // 1 MB

    var dispatcher = addon.Dispatcher
      .fromDevice(dev, maxFrameSize, monitor, promisc, timeout, bufferSize)
      .setFilter(filter);

    stream.Duplex.call(this, {
      allowHalfOpen: false,
      objectMode: true,
      highWaterMark: 1
    });
    Capture.call(this, dispatcher, opts, function (cb) { return cb; });

    this.getDevice = function () { return dev; };

    this.getStats = function () { return dispatcher.getStats(); };

    this._write = function (data, encoding, callback) {

      callback(); // TODO.

    };

  }
  util.inherits(Live, stream.Duplex);

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

  function getDefaultDevice() {

    return addon.getDefaultDevice();

  }

  function getLinkInfo(linkType) {

    return addon.getLinkInfo(linkType);

  }

  // Export things.

  root.exports = {
    Live: Live,
    Replay: Replay,
    Save: Save,
    getDefaultDevice: getDefaultDevice,
    getLinkInfo: getLinkInfo,
    summarize: summarize
  };

})(module);
