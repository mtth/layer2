/* jshint node: true */

(function () {
  'use strict';

  var level2 = require('../src/js');

  var capture = new level2.capture.Live(null, {monitor: true});
  var decoder = new level2.Decoder();

  capture
    .pipe(decoder)
    .on('data', function (frame) { console.log(JSON.stringify(frame)); });

})();
