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

  function decode(buf) {
    // Note that Radiocap frames are assumed to be always valid (they are added
    // locally, so aren't subject to corruption from transmission over the
    // noisy network).

    var frame = {};

    frame.headerRevision = buf[0];
    frame.headerPad = buf[1];
    frame.headerLength = buf.readUInt16LE(2);
    // TODO: Decode other radiotap fields.
    frame.body = IEEE802_11(buf.slice(frame.headerLength, buf.length));

    return frame;

  }

  root.exports = decode;

})(module);
