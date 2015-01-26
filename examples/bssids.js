/* jshint node: true */

/**
 * This script will run for 10 seconds and print the list of discovered BSSIDs
 * (i.e. typically MAC addresses) along with the associated number of frames
 * captured.
 *
 */
(function () {
  'use strict';

  var layer2 = require('../lib');

  var device = layer2.capture.getDefaultDevice();
  var capture = new layer2.capture.Live(device, {monitor: false});
  var decoder = new layer2.Decoder();
  var bssids = {};

  capture
    .close(10000)
    .pipe(decoder)
    .on('data', function (frame) {
      var pduType = layer2.pduTypes.ETHERNET_II;
      var pdu = frame.getPdu(pduType);
      if (pdu) {
        console.log(pdu.getSrcAddr() + ' -> ' + pdu.getDstAddr());
        console.log('size: ' + pdu.getSize() + ' type: ' + pdu.getPayloadType());
        // bssids[bssid] = (bssids[bssid] || 0) + 1;
      }
    })
    .on('end', function () { console.dir(bssids); });

})();
