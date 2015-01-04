/* jshint node: true */

/**
 * Outputs the total number of data bytes received per MAC address in the last
 * 10 seconds.
 *
 */
(function () {
  'use strict';

  var dot11 = require('../src/js');

  var capture = new dot11.capture.Live(null, {monitor: true});
  var decoder = new dot11.Decoder();
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
