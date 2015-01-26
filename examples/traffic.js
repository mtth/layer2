/* jshint node: true */

/**
 * Outputs the total number of data bytes received per MAC address in the last
 * 10 seconds.
 *
 */
(function () {
  'use strict';

  var layer2 = require('../lib');

  var device = layer2.capture.getDefaultDevice();
  var capture = new layer2.capture.Live(device, {monitor: true});
  var decoder = new layer2.Decoder();
  var nBytesReceived = {};

  capture
    .close(10000)
    .pipe(decoder)
    .on('data', function (frame) {
      var pdu = frame.getPdu(layer2.pduTypes.DOT11_QOS_DATA);
      if (pdu) {
        // TODO
        // var sa = frame.sa;
        // nBytesReceived[sa] = (nBytesReceived[sa] || 0) + frame.body.length;
      }
    })
    .on('end', function () { console.dir(nBytesReceived); });

})();
