/* jshint node: true */

(function () {
  'use strict';

  var dot11 = require('../lib');

  var fPath = process.argv[2];

  perf(fPath, {});

  function perf(path, opts, cb) {

    var sTime = process.hrtime();
    var nPackets = 0;
    var capture = new dot11.capture.Replay(path, opts);
    capture
      .on('data', function () { nPackets++; })
      .on('end', function () {
        var tTime = process.hrtime(sTime);
        var totalMs = tTime[0] * 1000 + tTime[1] / 1000000;
        var usPerPacket = 1000 * totalMs / nPackets;
        console.log(
          'dot11\t' +
          nPackets + '\t' +
          usPerPacket + ' us/p\t' +
          totalMs + ' ms\t' +
          path
        );
        if (cb) {
          cb();
        }
      });

  }

})();
