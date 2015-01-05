/* jshint node: true */

/**
 * Outputs the total number of data bytes received per MAC address in the last
 * 10 seconds.
 *
 */
(function () {
  'use strict';

  var layer2 = require('../src/js');

  var capture = new layer2.capture.Live(null, {monitor: true});
  var decoder = new layer2.Decoder();
  var nBytesReceived = {};

  capture
    .close(10000)
    .pipe(decoder)
    .on('data', function (frame) {
      frame = frame.body; // Extract 802.11 frame from Radiotap header.
      if (frame.type === 'data' && frame.body) {
        var sa = frame.sa;
        nBytesReceived[sa] = (nBytesReceived[sa] || 0) + frame.body.length;
      }
    })
    .on('end', function () { console.dir(nBytesReceived); });

})();
