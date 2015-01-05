/* jshint node: true */

/**
 * This script will run for 10 seconds and print the list of discovered BSSIDs
 * (i.e. typically MAC addresses) along with the associated number of frames
 * captured.
 *
 */
(function () {
  'use strict';

  var level2 = require('../src/js');

  var capture = new level2.capture.Live(null, {monitor: true});
  var decoder = new level2.Decoder();
  var bssids = {};

  capture
    .close(10000)
    .pipe(decoder)
    .on('data', function (frame) {
      var bssid = frame.body.bssid;
      if (bssid) {
        bssids[bssid] = (bssids[bssid] || 0) + 1;
      }
    })
    .on('end', function () { console.dir(bssids); });

})();
