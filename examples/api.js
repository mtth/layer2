/* jshint node: true */

'use strict';

var layer2 = require('../lib');

var sniffer = layer2.createInterfaceSniffer('en0', {bufferSize: 1024, rfmon: true, timeout: 0, filter: undefined});

sniffer
  .on('pdu', function (val) { console.log(val.$toString()); });
