/* jshint node: true */

/**
 * This script will run for 10 seconds and print the list of discovered BSSIDs
 * (i.e. typically MAC addresses) along with the associated number of frames
 * captured.
 *
 */
(function () {
  'use strict';

  var layer2 = require('../src/js');

  var device = layer2.capture.getDefaultDevice();
  var capture = new layer2.capture.Live(device, {monitor: true});
  var decoder = new layer2.Decoder();
  var bssids = {};

  capture
    .close(10000)
    .pipe(decoder)
    .on('data', function (frame) {
      var pdu = frame.getPdu(layer2.pduTypes.DOT11_BEACON);
      if (pdu) {
        // bssids[bssid] = (bssids[bssid] || 0) + 1;
      }
    })
    .on('end', function () { console.dir(bssids); });

})();
