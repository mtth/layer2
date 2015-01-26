/* jshint node: true */

/**
 * Benchmark various MAC address parsing methods.
 *
 */
(function () {
  'use strict';

  var utils = require('./utils'),
      addon = require('../lib/utils').requireAddon();

  var benchmark = new utils.Benchmark();

  benchmark
    .addFn('pcap', wrap(function (buf, offset) {

      return [
        lpad(buf[offset].toString(16), 2),
        lpad(buf[offset + 1].toString(16), 2),
        lpad(buf[offset + 2].toString(16), 2),
        lpad(buf[offset + 3].toString(16), 2),
        lpad(buf[offset + 4].toString(16), 2),
        lpad(buf[offset + 5].toString(16), 2)
      ].join(':');

  }))
  .addFn('toString concat', wrap(function (buf, offset) {

    return buf.toString('hex', offset, offset + 1) + ':' +
      buf.toString('hex', offset + 1, offset + 2) + ':' +
      buf.toString('hex', offset + 2, offset + 3) + ':' +
      buf.toString('hex', offset + 3, offset + 4) + ':' +
      buf.toString('hex', offset + 4, offset + 5) + ':' +
      buf.toString('hex', offset + 5, offset + 6);

  }))
  .addFn('toString join', wrap(function (buf, offset) {

    return [
      buf.toString('hex', offset, offset + 1),
      buf.toString('hex', offset + 1, offset + 2),
      buf.toString('hex', offset + 2, offset + 3),
      buf.toString('hex', offset + 3, offset + 4),
      buf.toString('hex', offset + 4, offset + 5),
      buf.toString('hex', offset + 5, offset + 6)
    ].join(':');

  }))
  .addFn('slice join', wrap(function (buf, offset) {

    var s = buf.toString('hex', offset, offset + 6);
    return [
      s.slice(0, 2),
      s.slice(2, 4),
      s.slice(4, 6),
      s.slice(6, 8),
      s.slice(8, 10),
      s.slice(10, 12)
    ].join(':');

  }))
  .addFn('cpp function', wrap(function (buf, offset) {

    return addon.readMacAddr(buf, offset);

  }))
  .run(1e2, function (stats) {
    console.dir(stats);
  });

  // Helpers.

  function wrap(decoder) {

    var buf = new Buffer('80000000ffffffffffff06037f07a016', 'hex');

    return function (cb) {

      for (var i = 0; i < 1e4; i++) {
        decoder(buf, 10);
      }
      cb();

    };

  }

  function lpad(str, len) {
    while (str.length < len) {
      str = '0' + str;
    }
    return str;
  }

})();
