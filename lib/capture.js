/* jshint node: true */

/**
 * Frame capture and injection streams.
 *
 */
(function (root) {
  'use strict';

  var util = require('util'),
      stream = require('stream'),
      utils = require('./utils');

  var addon = utils.requireAddon();

  /**
   * Live frame capture stream.
   *
   */
  function Live(device, opts) {

    opts = opts || {};
    var monitor = opts.monitor || false;
    var promisc = opts.promisc || false;
    var filter = opts.filter || '';
    var timeout = opts.timeout || 100;
    var maxFrameSize = opts.maxFrameSize || 65535; // 65 kB
    var bufferSize = opts.bufferSize || 1024 * 1024; // 1 MB
    var batchSize = opts.batchSize || -1;

    var dispatcher = addon.Dispatcher
      .fromDevice(device, maxFrameSize, monitor, promisc, timeout, bufferSize)
      .setFilter(filter);

    utils.BatchStream.call(this, {
      objectMode: true,
      highWaterMark: 2,
      batchHighWatermark: 2,
      concurrency: 1,
      duplex: true
    });

    this._batch = function (cb) {

      dispatcher.dispatch(batchSize, function (err, iter) {
        cb(err, new utils.Iterator(iter));
      });

    };

    this._write = function (data, encoding, callback) {

      dispatcher.inject(data);
      callback();

    };

    this.on('finish', function () { this.finish(); });

    this.on('end', function () {

      process.nextTick(dispatcher.close.bind(dispatcher));

    });

    // "Public" methods.

    this.getDevice = function () { return device; };

    this.getStats = function () { return dispatcher.getStats(); };

    this.getLinkType = function () { return dispatcher.getDatalink(); };

    this.getMaxFrameSize = function () { return dispatcher.getSnaplen(); };

  }
  util.inherits(Live, stream.Duplex);

  /**
   * Frame capture stream from saved file.
   *
   */
  function Replay(fpath, opts) {

    opts = opts || {};
    var filter = opts.filter || '';
    var batchSize = opts.batchSize || 500;

    var dispatcher = addon.Dispatcher
      .fromSavefile(fpath)
      .setFilter(filter);

    utils.BatchStream.call(this, {
      objectMode: true,
      highWaterMark: 2,
      batchHighWatermark: 2,
      concurrency: 1
    });

    this._batch = function (cb) {

      dispatcher.dispatch(batchSize, function (err, iter, nFrames) {
        if (nFrames) {
          cb(err, new utils.Iterator(iter));
        } else { // EOF
          cb(err, null);
        }
      });

    };

    this.on('end', function () {

      process.nextTick(dispatcher.close.bind(dispatcher));

    });

    // "Public" methods.

    this.getPath = function () { return fpath; };

    this.getLinkType = function () { return dispatcher.getDatalink(); };

    this.getMaxFrameSize = function () { return dispatcher.getSnaplen(); };

  }
  util.inherits(Replay, stream.Readable);

  /**
   * Save capture to file.
   *
   */
  function Save(fpath, opts) {

    opts = opts || {};
    var linkType = opts.linkType; // Inferred below if unspecified.
    var maxFrameSize = opts.maxFrameSize || 65535;

    stream.Writable.call(this, {objectMode: true});

    var dispatcher = null;

    this.on('pipe', function (src) {
      // Infer link type from first capture stream piped.

      if (linkType === undefined) {
        linkType = src.getLinkType();
      } else if (linkType !== src.getLinkType()) {
        this.emit('error', new Error('Inconsistent link type.'));
      }

    });

    this.on('finish', function () {
      // Force flush of frames (otherwise this would only happen when the
      // writable stream gets garbage collected). Note also that it's important
      // to attach this handler here (and not when the dispatcher is
      // instantiated for example), otherwise this handler won't be
      // guaranteed to be called first.

      if (dispatcher) {
        dispatcher.setSavefile(null);
      }

    });

    this._write = function (data, encoding, callback) {

      if (!dispatcher) {
        if (linkType === null) {
          return callback(new Error('No link type specified.'));
        }
        dispatcher = addon.Dispatcher
          .fromDead(linkType, maxFrameSize)
          .setSavefile(fpath);
      }

      dispatcher.dump(data);
      return callback();

    };

    // "Public" methods.

    this.getPath = function () { return fpath; };

    this.getLinkType = function () { return linkType; };

    this.getMaxFrameSize = function () { return maxFrameSize; };

  }
  util.inherits(Save, stream.Writable);

  // Helpers.

  function summarize(fpath, cb) {

    var nFrames = 0;
    var nBytes = 0;
    var replay = new Replay(fpath);

    replay
      .on('data', function (frame) {
        nFrames++;
        nBytes += frame.size();
      })
      .on('error', function (err) {
        cb(err);
      })
      .on('end', function () {
        var summary = {
          linkType: replay.getLinkType(),
          maxFrameSize: replay.getMaxFrameSize(),
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
    getDefaultDevice: addon.getDefaultDevice,
    getLinkInfo: addon.getLinkInfo,
    summarize: summarize
  };

})(module);
