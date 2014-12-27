/* jshint node: true */

(function () {
  'use strict';

  var pcap = require('pcap');

  var fPath = process.argv[2];

  perf(fPath);

  function perf(path) {

    var sTime = process.hrtime();
    var nPackets = 0;
    var session = pcap.createOfflineSession(path);
    session
      .on('packet', function () { nPackets++; })
      .on('complete', function () {
        var tTime = process.hrtime(sTime);
        var totalMs = tTime[0] * 1000 + tTime[1] / 1000000;
        var usPerPacket = 1000 * totalMs / nPackets;
        console.log(
          'pcap\t' +
          nPackets + '\t' +
          usPerPacket + ' us/p\t' +
          totalMs + ' ms\t' +
          path
        );
      });

  }

})();
