/* jshint node: true */

/**
 * Radiotap header decoder.
 *
 * Sources:
 *
 * + http://www.radiotap.org/
 *
 */
(function (root) {
  'use strict';

  var IEEE802_11 = require('./IEEE802_11');

  function decode(buf, assumeValid) {
    // Note that Radiocap frames are assumed to always be valid (they are added
    // locally, so aren't subject to corruption from transmission over the
    // noisy network).

    if (!assumeValid) {
      var presentFlags = buf.readUInt32LE(4);
      var offset = 8;
      if (presentFlags & 0x01) { // TSFT.
        offset += 8;
      }
      if (presentFlags & 0x02) { // Flags.
        var flags = buf[offset];
        var badFcs = (flags >>> 6) & 0x01;
        if (badFcs) {
          return null;
        } else {
          assumeValid = true; // The FCS has already been checked.
        }
      }
    }

    var headerLength = buf.readUInt16LE(2);
    var body = IEEE802_11(buf.slice(headerLength, buf.length), assumeValid);
    if (body === null) {
      return null;
    } else {
      return {
        headerLength: headerLength,
        headerRevision: buf[0],
        headerPad: buf[1],
        body: body
      };
    }

  }

  root.exports = decode;

})(module);
