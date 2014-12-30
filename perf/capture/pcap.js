/* jshint node: true */

(function () {
  'use strict';

  var pcap = require('pcap'),
      setup = require('./setup');

  var fPath = process.argv[2];

  perf(fPath);

  function perf(path) {

    var sTime = process.hrtime();
    var nPackets = 0;
    var session = pcap.createOfflineSession(path);
    session
      .on('packet', function () { nPackets++; })
      .on('complete', function () {
        setup.displayResults(
          'pcap',
          process.hrtime(sTime),
          nPackets,
          path
        );
      });

  }

})();
