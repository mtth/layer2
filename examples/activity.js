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

  var level2 = require('../src/js'),
      util = require('util');

  var capture = new level2.capture.Live(null, {monitor: true});
  var decoder = new level2.Decoder();
  var nValid = 0;
  var nInvalid = 0;

  capture
    .once('readable', function () {
      console.log(util.format(
        '\nListening on device %s (link type: %s).\n',
        this.getDevice(), this.getLinkType()
      ));
    })
    .pipe(decoder)
    .on('data', function () {
      nValid++;
      process.stderr.write('.');
    })
    .on('invalid', function () {
      nInvalid++;
      process.stderr.write('I');
    });

  process.on('SIGINT', function () {
    capture
      .on('close', function () {
        console.log(util.format(
          '\n\nTotal frames captured: %s (%s%% invalid).',
          (nValid + nInvalid),
          (100 * nInvalid / (nValid + nInvalid)).toFixed(1)
        ));
      })
      .close();
  });

})();
