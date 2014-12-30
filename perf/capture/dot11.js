/* jshint node: true */

(function () {
  'use strict';

  var dot11 = require('../../lib'),
      setup = require('./setup');

  var fPath = process.argv[2];

  perf(fPath, {});

  function perf(path) {

    var sTime = process.hrtime();
    var nFrames = 0;
    var capture = new dot11.capture.Replay(path);
    capture
      .on('data', function () { nFrames++; })
      .on('end', function () {
        setup.displayResults(
          'dot11',
          process.hrtime(sTime),
          nFrames,
          path
        );
      });

  }

})();
