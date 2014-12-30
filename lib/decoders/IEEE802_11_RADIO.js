/* jshint node: true */

(function (root) {
  'use strict';

  function Transform(opts) {

    opts = opts || {};
    var stringify = opts.stringify || false;

    this._transform = function (data, encoding, callback) {

      var packet;
      try {
        packet = decode(data);
        if (stringify) {
          packet = JSON.stringify(packet);
        }
      } catch (err) {
        err.data = data; // Attach raw bytes to error.
        this.emit('invalid', err);
        return callback();
      }
      return callback(null, packet);

    };

  }

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

  root.exports = {
    Transform: Transform,
    decode: decode
  };

})(module);
