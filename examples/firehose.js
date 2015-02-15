/* jshint node: true */

(function () {
  'use strict';

  var layer2 = require('../lib');

  var device = layer2.capture.getDefaultDevice() || 'en0';
  var capture = new layer2.capture.Live(device, {monitor: true});

  capture
    .on('data', function (frame) { console.log(frame); });

})();
