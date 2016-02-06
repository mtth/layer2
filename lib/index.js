/* jshint node: true */

'use strict';

var sniffers = require('./sniffers');


module.exports = {
  createInterfaceSniffer: sniffers.createInterfaceSniffer,
  createFileSniffer: sniffers.createFileSniffer
};
