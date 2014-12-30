/* jshint node: true */

(function () {
  'use strict';

  var dot11 = require('../../lib'),
      setup = require('./setup');

  var fPath = process.argv[2];

  perf(fPath, {});

  function perf(path) {

    var sTime = process.hrtime();
    var nPackets = 0;
    var capture = new dot11.capture.Replay(path);
    capture
      .on('data', function () { nPackets++; })
      .on('end', function () {
        setup.displayResults(
          'dot11',
          process.hrtime(sTime),
          nPackets,
          path
        );
      });

  }

})();
