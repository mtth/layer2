/* jshint node: true */

(function (root) {
  'use strict';

  function decode(buf) {

    var frame = {};
    frame.headerRevision = buf[0];
    frame.headerPad = buf[1];
    frame.headerLength = buf[2] + 256 * buf[3];
    // TODO: Decode other radiotap fields (cf. http://www.radiotap.org/).
    var bodyOffset = frame.headerLength;
    frame.body = new Buffer(buf.length - bodyOffset);
    buf.copy(frame.body, 0, bodyOffset);
    // TODO: Try slicing instead of copying? The issue with slicing is that we
    // might end up keeping many buffers around even though we are only using a
    // small part of their memory which would also lead to very fragmented
    // memory. By copying we use more memory in the short term but allow the
    // previous buffers to be reclaimed.
    return frame;

  }

  root.exports.decode = decode;

})(module);
