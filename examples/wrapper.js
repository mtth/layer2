/* jshint node: true */

'use strict';

var layer2 = require('../lib');

var wrapper = new layer2.Wrapper().fromInterface('en0');
var buf = new Buffer(128);
buf.fill(0);
wrapper.getPdus(buf, function (err, n) {
  console.error(err);
  console.log(n);
  console.log(buf);
});
