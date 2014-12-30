/* jshint node: true */

/**
 * Various utilities.
 *
 * (If only the built-in`util` package had a different name...)
 *
 */
(function (root) {
  'use strict';

  function readMacAddr(buf, offset) {

    var s = buf.toString('hex', offset, offset + 6);

    if (s.length < 12) {
      throw new Error('Truncated MAC address at offset ' + offset);
    }

    return [
      s.slice(0, 2),
      s.slice(2, 4),
      s.slice(4, 6),
      s.slice(6, 8),
      s.slice(8, 10),
      s.slice(10, 12)
    ].join(':');

  }

  root.exports = {
    readMacAddr: readMacAddr
  };

})(module);
