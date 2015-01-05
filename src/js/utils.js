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

  /**
   * Ha.
   *
   */
  function Packet(type, contents) {

    this.type = type;
    this.contents = contents;

  }

  function readMacAddr(buf, offset) {
    // MAC address parser.
    // Returns a formatted address such as 00:01:02:03:04:05.

    var s = buf.toString('hex', offset, offset + 6);

    return [
      s.slice(0, 2),
      s.slice(2, 4),
      s.slice(4, 6),
      s.slice(6, 8),
      s.slice(8, 10),
      s.slice(10, 12)
    ].join(':');

  }

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
    Packet: Packet,
    readMacAddr: readMacAddr,
    crc32: makeCrc32(),
    requireDirectory: requireDirectory
  };

})(module);
