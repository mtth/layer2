/* jshint node: true */

(function (root) {
  'use strict';

  root.exports = {
    capture: require('./capture'),
    Decoder: require('./decode').Decoder
  };

})(module);
