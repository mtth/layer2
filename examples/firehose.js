/* jshint node: true */

(function () {
  'use strict';

  var layer2 = require('../src/js');

  var capture = new layer2.capture.Live(null, {monitor: true});
  var decoder = new layer2.Decoder();

  capture
    .pipe(decoder)
    .on('data', function (frame) { console.log(JSON.stringify(frame)); });

})();
