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

  function Transform(opts) {

    opts = opts || {};
    var stringify = opts.stringify || false;

    this._transform = function (data, encoding, callback) {

      var frame;

      try {
        // Note that Radiocap frames are assumed to be always valid (they are
        // added locally, so aren't subject to corruption from transmission
        // over the noisy network).
        frame = decode(data);
        if (stringify) {
          frame = JSON.stringify(frame);
        }
      } catch (err) {
        err.data = data; // Attach raw bytes to error.
        return callback(err);
      }
      return callback(null, frame);

    };

  }

  function decode(buf) {

    var frame = {};

    frame.headerRevision = buf[0];
    frame.headerPad = buf[1];
    frame.headerLength = buf.readUInt16LE(2);
    // TODO: Decode other radiotap fields.

    var bodyOffset = frame.headerLength;
    frame.body = new Buffer(buf.length - bodyOffset);
    buf.copy(frame.body, 0, bodyOffset);
    // Maybe try slicing instead of copying here? The issue with slicing is
    // that we might end up keeping many buffers around even though we are only
    // using a small part of their memory which would also lead to very
    // fragmented memory. By copying we use more memory in the short term but
    // allow the previous buffers to be reclaimed.

    return frame;

  }

  root.exports = {
    Transform: Transform,
    decode: decode
  };

})(module);
