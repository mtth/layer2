/* jshint node: true */

'use strict';

var layer2 = require('../lib'),
    util = require('util');

var sniffer = layer2.createInterfaceSniffer('en0', {bufferSize: 1024, rfmon: true, timeout: 0, filter: undefined});
var n = 100;

sniffer
  .on('pdu', function (val) {
    console.log(util.inspect(val, {depth: null}));
    if (!--n) {
      this.destroy();
    }
  })
  .on('end', function () { console.log('done'); });
