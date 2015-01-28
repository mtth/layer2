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
   * Live frame capture stream.
   *
   */
  function Live(dev, opts) {

    opts = opts || {};
    var maxFrameSize = opts.maxFrameSize || 65535;
    var monitor = opts.monitor || false;
    var promisc = opts.promisc || false;
    var timeout = opts.timeout || 100;
    var bufferSize = opts.bufferSize || 1024 * 1024;
    var filter = opts.filter || '';

    var dispatcher = addon.Dispatcher
      .fromDevice(dev, maxFrameSize, monitor, promisc, timeout, bufferSize)
      .setFilter(filter);

    this.getDevice = function () { return dev; };

    this.getStats = function () { return dispatcher.getStats(); };

    this.getLinkInfo = function () { return dispatcher.getDatalink(); }; // TODO.

    this.getMaxFrameSize = function () { return dispatcher.getSnaplen(); };

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

  // Export things.

  root.exports = {
    Live: Live,
  };

})(module);
