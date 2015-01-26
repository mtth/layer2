/* jshint node: true */

(function (root) {
  'use strict';

  var decode = require('./decode');

  root.exports = {
    capture: require('./capture'),
    Decoder: decode.Decoder,
    Frame: decode.Frame,
    getPduName: decode.getPduName,
    pduTypes: decode.pduTypes
  };

})(module);
