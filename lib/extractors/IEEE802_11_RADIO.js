/* jshint node: true */

(function (root) {
  'use strict';

  function IEEE802_11(opts) {

    opts = opts || {};

    var needPacket = false;
    var packet = null;

    this._read = function () {

      if (packet) {
        var buf = packet;
        var frame = null;
        packet = null;
        try {
          frame = extract(buf);
        } catch (err) {
          needPacket = true;
          err.data = buf; // Attach buffer to error.
          return this.emit('invalid', err);
        }
        needPacket = false;
        this.push(frame);
      } else {
        needPacket = true;
      }

    };

    this._write = function (data, encoding, callback) {

      packet = data;
      if (needPacket) {
        this._read();
      }
      return callback();

    };

    function extract(buf) {

      var headerLength = buf[2] + 256 * buf[3];
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
