/* jshint node: true */

(function (root) {
  'use strict';

  function IEEE802_11_FRAME(opts) {
    // Basic implementation of a sync transform object stream (i.e. where the
    // `_transform` function is always synchronous).

    var ignoreErrors = opts.ignoreErrors || false;

    var needPacket = false;
    var packet = null;

    this._read = function () {

      if (packet) {
        var buf = packet;
        var frame;
        packet = null;
        needPacket = false;
        try {
          frame = extract(buf);
        } catch (err) {
          if (ignoreErrors) {
            needPacket = true;
          } else {
            this.emit(err);
          }
        }
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

  }

  root.exports.IEEE802_11_FRAME = IEEE802_11_FRAME;

})(module);
