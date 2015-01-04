/* jshint node: true */

(function () {
  'use strict';

  var dot11 = require('../src/js');

  var capture = new dot11.capture.Live(null, {monitor: true});
  var decoder = new dot11.Decoder();

  capture
    .pipe(decoder)
    .on('data', function (frame) { console.log(JSON.stringify(frame)); });

})();
