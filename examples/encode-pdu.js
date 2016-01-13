/* jshint node: true */

'use strict';

var layer2 = require('../lib'),
    avro = require('avsc');

var registry = {};
avro.parse('./pdus.avsc', {registry: registry});
// console.log(registry);

var type = registry.Pdu;

var buf = new Buffer(256);
buf.fill(0);
var n = layer2.test(buf);

console.log('found ' + n + ' frames');
var pos = 0;
var obj;
while (n--) {
  obj = type.decode(buf, pos);
  console.log(obj.value.pdu);
  pos = obj.offset;
}
console.log(buf);
