/* jshint node: true */

/**
 * 802.11 overall activity.
 *
 * Runs until interrupted.
 *
 * Also output total frames and fraction of invalid frames at the end. Note
 * that this measures total number of frames captured, not the total data size
 * (in particular, this includes all control and management frames).
 *
 */
(function () {
  'use strict';

  var layer2 = require('../lib'),
      util = require('util');

  var device = layer2.capture.getDefaultDevice() || 'en0';
  var capture = new layer2.capture.Live(device, {monitor: true});
  var nValid = 0;
  var nInvalid = 0;
  var startTime;

  capture
    .once('readable', function () {
      console.log(util.format(
        '\nListening on device %s (link type: %s).\n',
        this.getDevice(), layer2.capture.getLinkInfo(this.getLinkType()).name
      ));
      startTime = process.hrtime();
    })
    .on('data', function (frame) {
      if (frame.isValid()) {
        nValid++;
        process.stderr.write('.');
      } else {
        nInvalid++;
        process.stderr.write('I');
      }
    })
    .on('end', function () {
      var runTime = process.hrtime(startTime);
      console.log(util.format(
        '\n\n%s frames captured in %s seconds (%s%% invalid).',
        (nValid + nInvalid),
        (runTime[0] + 1e-9 * runTime[1]).toFixed(1),
        (100 * nInvalid / (nValid + nInvalid)).toFixed(1)
      ));
    });

  process.on('SIGINT', function () { capture.end(); });

})();
