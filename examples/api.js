/* jshint node: true */

'use strict';

var layer2 = require('../lib');

var sniffer = layer2.createInterfaceSniffer('en0');

sniffer
  .on('pdu', function (val) { console.log(val.$toString()); });
