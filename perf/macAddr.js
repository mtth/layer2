/* jshint node: true */

/**
 * Benchmark various MAC address parsing methods.
 *
 */
(function () {
  'use strict';

  var Benchmark = require('benchmark'),
      util = require('util');

  var suite = new Benchmark.Suite();

  function benchmark(label, decoder) {

    var buf = new Buffer('80000000ffffffffffff06037f07a016', 'hex');

    return {
      name: label,
      fn: function () { decoder(buf, 10); }
    };

  }

  suite.add(benchmark('pcap', function (buf, offset) {

    return [
      lpad(buf[offset].toString(16), 2),
      lpad(buf[offset + 1].toString(16), 2),
      lpad(buf[offset + 2].toString(16), 2),
      lpad(buf[offset + 3].toString(16), 2),
      lpad(buf[offset + 4].toString(16), 2),
      lpad(buf[offset + 5].toString(16), 2)
    ].join(':');

  }));

  suite.add(benchmark('1', function (buf, offset) {

    return buf.toString('hex', offset, offset + 1) + ':' +
      buf.toString('hex', offset + 1, offset + 2) + ':' +
      buf.toString('hex', offset + 2, offset + 3) + ':' +
      buf.toString('hex', offset + 3, offset + 4) + ':' +
      buf.toString('hex', offset + 4, offset + 5) + ':' +
      buf.toString('hex', offset + 5, offset + 6);

  }));

  suite.add(benchmark('2', function (buf, offset) {

    return buf.toString('hex', offset, offset + 1) + ':' +
      buf.toString('hex', offset + 1, offset + 2) + ':' +
      buf.toString('hex', offset + 2, offset + 3) + ':' +
      buf.toString('hex', offset + 3, offset + 4) + ':' +
      buf.toString('hex', offset + 4, offset + 5) + ':' +
      buf.toString('hex', offset + 5, offset + 6);

  }));

  suite.add(benchmark('3', function (buf, offset) {

    return [
      buf.toString('hex', offset, offset + 1),
      buf.toString('hex', offset + 1, offset + 2),
      buf.toString('hex', offset + 2, offset + 3),
      buf.toString('hex', offset + 3, offset + 4),
      buf.toString('hex', offset + 4, offset + 5),
      buf.toString('hex', offset + 5, offset + 6)
    ].join(':');

  }));

  suite.add(benchmark('4', function (buf, offset) {

    var s = buf.toString('hex', offset, offset + 6);
    return [
      s.slice(0, 2),
      s.slice(2, 4),
      s.slice(4, 6),
      s.slice(6, 8),
      s.slice(8, 10),
      s.slice(10, 12)
    ].join(':');

  }));

  suite
    .on('cycle', function () { process.stderr.write('.'); })
    .on('complete', function () {
      var bs = this.sort(function (b1, b2) { return b2.hz - b1.hz; });
      console.error(util.format('\n1.\t\t%s', bs[0]));
      var i;
      for (i = 1; i < bs.length; i++) {
        var b = bs[i];
        var percent = Math.round((1 - (b.hz / bs[0].hz)) * 100);
        console.error(util.format('%s.\t-%s%%\t%s', i + 1, percent, b));
      }
    })
    .run();

  // Helpers.

  function lpad(str, len) {
    while (str.length < len) {
      str = '0' + str;
    }
    return str;
  }

})();
