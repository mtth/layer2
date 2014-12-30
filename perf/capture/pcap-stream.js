/* jshint node: true */

(function () {
  'use strict';

  var PcapStream = require('pcap-stream'),
      setup = require('./setup');

  var fPath = process.argv[2];

  perf(fPath);

  function perf(path) {

    var sTime = process.hrtime();
    var nPackets = 0;
    var pStream = new PcapStream(path);
    pStream
      .on('data', function () { nPackets++; })
      .on('end', function () {
        setup.displayResults(
          'pcap-stream',
          process.hrtime(sTime),
          nPackets,
          path
        );
      });

  }

})();
