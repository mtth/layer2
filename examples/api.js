/* jshint node: true */

'use strict';

var layer2 = require('../lib'),
    util = require('util');

var sniffer = layer2.createInterfaceSniffer('en0', {rfmon: false, bufferSize: 1024});
var n = 100;

sniffer
  .on('pdu', function (val) {
    // console.log(util.inspect(val, {depth: null}));
    console.log(val.pdu.Ethernet2.data.toString('binary'));
    if (!--n) {
      this.destroy();
    }
  })
  .on('end', function () { console.log('done'); });
