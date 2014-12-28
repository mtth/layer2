/* jshint node: true */

/**
 * Import all extractors inside this folder. The name of the file should
 * correspond to the link type it extracts _from_.
 *
 * An extractor is a "class" that provides `_read` and `_write` methods
 * (compatible with the duplex stream interface). It takes as constructor
 * arguments a single dictionary of options.
 *
 */
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
