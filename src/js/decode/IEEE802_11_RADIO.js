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

  function decode(buf, opts) {
    // Note that Radiocap frames are assumed to be always valid (they are added
    // locally, so aren't subject to corruption from transmission over the
    // noisy network).

    var headerLength = buf.readUInt16LE(2);
    // TODO: use FCS field in radiotap header to skip CRC check in wifi
    // decoder.
    var body = IEEE802_11(buf.slice(headerLength, buf.length), opts);

    if (body === null) {
      // Don't waste time decoding anything if the contained frame is invalid.
      return null;
    } else {
      // TODO: Decode other radiotap fields.
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
