/* jshint node: true */

(function (root) {
  'use strict';

  var fs = require('fs');

  var names = fs.readdirSync(__dirname);
  var pattern = /([^.]*).js/;

  var i, match;
  for (i = 0; i < names.length; i++) {
    match = pattern.exec(names[i]);
    if (match && match[1] !== 'index') {
      root.exports[match[1]] = require('./' + match[1]);
    }
  }

})(module);
