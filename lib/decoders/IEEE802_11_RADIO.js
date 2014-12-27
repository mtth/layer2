/* jshint node: true */

(function (root) {
  'use strict';

  function decode(buf, offset) {

    offset = offset || 0;

    var frame = {};
    frame.headerRevision = buf[offset];
    frame.headerPad = buf[offset + 1];
    frame.headerLength = uint16_be(buf, offset + 2);
    // TODO: Decode other radiotap fields (cf. http://www.radiotap.org/).
    var bodyOffset = offset + frame.headerLength;
    frame.body = new Buffer(buf.length - bodyOffset);
    buf.copy(frame.body, 0, bodyOffset);
    // TODO: Try slicing instead of copying? The issue with slicing is that we
    // might end up keeping many buffers around even though we are only using a
    // small part of their memory which would also lead to very fragmented
    // memory. By copying we use more memory in the short term but allow the
    // previous buffers to be reclaimed.
    return frame;

  }

  function uint16_be(buf, offset) {

    return buf[offset+1] * 256 + buf[offset];

  }

  root.exports.decode = decode;

})(module);
