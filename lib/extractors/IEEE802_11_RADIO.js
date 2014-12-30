/* jshint node: true */

(function (root) {
  'use strict';

  function IEEE802_11(opts) {

    opts = opts || {};

    var needFrame = false;
    var frame = null;

    this._read = function () {

      if (frame) {
        var outerFrame = frame;
        var innerFrame = null;
        frame = null;
        try {
          innerFrame = extract(outerFrame);
        } catch (err) {
          needFrame = true;
          err.data = outerFrame; // Attach buffer to error.
          return this.emit('invalid', err);
        }
        needFrame = false;
        this.push(innerFrame);
      } else {
        needFrame = true;
      }

    };

    this._write = function (data, encoding, callback) {

      frame = data;
      if (needFrame) {
        this._read();
      }
      return callback();

    };

    function extract(buf) {

      var headerLength = buf.readUInt16LE(2);
      var body = new Buffer(buf.length - headerLength);
      buf.copy(body, 0, headerLength);
      return body;

    }

    return extract; // For testing.

  }

  root.exports = {
    IEEE802_11: IEEE802_11, // Currently only one.
  };

})(module);
