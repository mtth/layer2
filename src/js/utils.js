/* jshint node: true */

/**
 * Various utilities.
 *
 * (If only the built-in`util` package had a different name...)
 *
 */
(function (root) {
  'use strict';

  var fs = require('fs'),
      path = require('path');

  function makeCrc32() {
    // Checksum computation.
    // http://stackoverflow.com/questions/18638900/javascript-crc32

    var crcTable = (function () {
      var table = [];
      var c, k, n;
      for (n = 0; n < 256; n++) {
        c = n;
        for (k = 0; k < 8; k++) {
          c = ((c & 0x01) ? (0xedb88320 ^ (c >>> 1)) : (c >>> 1));
        }
        table[n] = c;
      }
      return table;
    })();

    return function (buf) {
      var crc = -1;
      var i, l;
      for (i = 0, l = buf.length; i < l; i++ ) {
          crc = (crc >>> 8) ^ crcTable[(crc ^ buf[i]) & 0xff];
      }
      return ~crc >>> 0;
    };

  }

  function requireAddon() {
    // Either debug or release depending on environment variable (defaulting to
    // the existing one if no override).

    var buildRoot = path.join(__dirname, '..', '..', 'build');
    var override = process.env.LAYER2_DEBUG;
    var debug;

    if (override) {
      debug = parseInt(override);
    } else {
      debug = fs.existsSync(path.join(buildRoot, 'Debug'));
    }

    var buildFolder = debug ?
      path.join(buildRoot, 'Debug') :
      path.join(buildRoot, 'Release');

    if (!fs.existsSync(buildFolder)) {
      throw new Error('No addon found at: ' + buildFolder);
    } else {
      return require(buildFolder);
    }

  }

  function requireDirectory(dpath) {

    var names = fs.readdirSync(dpath);
    var pattern = /([^.]*).js/;
    var target = {};
    var i, match;
    for (i = 0; i < names.length; i++) {
      match = pattern.exec(names[i]);
      if (match && match[1] !== 'index') {
        target[match[1]] = require(path.join(dpath, match[1]));
      }
    }
    return target;

  }

  root.exports = {
    crc32: makeCrc32(),
    requireAddon: requireAddon,
    requireDirectory: requireDirectory
  };

})(module);
