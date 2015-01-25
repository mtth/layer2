/* jshint node: true */

(function (root) {
  'use strict';

  var addon = require('./utils').requireAddon();

  root.exports = {
    capture: require('./capture'),
    Decoder: require('./decode').Decoder,
    Frame: addon.Frame
  };

})(module);
