/* jshint node: true */

(function (root) {
  'use strict';

  function RAW(opts) {

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
          return this.emit('invalid', outerFrame, err);
        }
        if (innerFrame === null) {
          // Non IP Ethernet frame.
          needFrame = true;
          return this.emit('skip', outerFrame);
        }
        // Got an IP frame;
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

      var type = buf.readUInt16BE(12);
      switch (type) {
        case 0x0800: // ipv4
        case 0x86dd: // ipv6
          var data = new Buffer(buf.length - 18);
          buf.copy(data, 0, 14, buf.length - 4);
          return data;
        default:
          return null;
      }

    }

   return extract;

  }

  root.exports = {
    RAW: RAW
  };

})(module);
