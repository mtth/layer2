/* jshint node: true */

'use strict';

var layer2 = require('../lib');

var sniffer = layer2.createInterfaceSniffer('en0', {bufferSize: 1024, rfmon: true, timeout: 0, filter: undefined});
var n = 100;

sniffer
  .on('pdu', function (val) {
    console.log(val.$toString());
    if (!--n) {
      this.destroy();
    }
  })
  .on('end', function () { console.log('done'); });
