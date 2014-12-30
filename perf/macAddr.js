/* jshint node: true */

/**
 * Benchmark various MAC address parsing methods.
 *
 */
(function () {
  'use strict';

  var assert = require('assert');

  function benchmark(label, decoder) {

    var count = 1000000;
    var frame = new Buffer('80000000ffffffffffff06037f07a016', 'hex');
    // (802.11 beacon frame.)
    var addr, i;

    console.time(label);
    for (i = 0; i < count; i++) {
      addr = decoder(frame, 10);
    }
    assert.equal(addr, '06:03:7f:07:a0:16');
    console.timeEnd(label);

  }

  benchmark('pcap', function (buf, offset) {

    return [
      lpad(buf[offset].toString(16), 2),
      lpad(buf[offset + 1].toString(16), 2),
      lpad(buf[offset + 2].toString(16), 2),
      lpad(buf[offset + 3].toString(16), 2),
      lpad(buf[offset + 4].toString(16), 2),
      lpad(buf[offset + 5].toString(16), 2)
    ].join(':');

  });

  benchmark('1', function (buf, offset) {

    return buf.toString('hex', offset, offset + 1) + ':' +
      buf.toString('hex', offset + 1, offset + 2) + ':' +
      buf.toString('hex', offset + 2, offset + 3) + ':' +
      buf.toString('hex', offset + 3, offset + 4) + ':' +
      buf.toString('hex', offset + 4, offset + 5) + ':' +
      buf.toString('hex', offset + 5, offset + 6);

  });

  benchmark('2', function (buf, offset) {

    return buf.toString('hex', offset, offset + 1) + ':' +
      buf.toString('hex', offset + 1, offset + 2) + ':' +
      buf.toString('hex', offset + 2, offset + 3) + ':' +
      buf.toString('hex', offset + 3, offset + 4) + ':' +
      buf.toString('hex', offset + 4, offset + 5) + ':' +
      buf.toString('hex', offset + 5, offset + 6);

  });

  benchmark('3', function (buf, offset) {

    return [
      buf.toString('hex', offset, offset + 1),
      buf.toString('hex', offset + 1, offset + 2),
      buf.toString('hex', offset + 2, offset + 3),
      buf.toString('hex', offset + 3, offset + 4),
      buf.toString('hex', offset + 4, offset + 5),
      buf.toString('hex', offset + 5, offset + 6)
    ].join(':');

  });

  benchmark('4', function (buf, offset) {

    var s = buf.toString('hex', offset, offset + 6);
    return [
      s.slice(0, 2),
      s.slice(2, 4),
      s.slice(4, 6),
      s.slice(6, 8),
      s.slice(8, 10),
      s.slice(10, 12)
    ].join(':');

  });

  // Helpers.

  function lpad(str, len) {
    while (str.length < len) {
      str = '0' + str;
    }
    return str;
  }

})();
