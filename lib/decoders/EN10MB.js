/* jshint node: true */

/**
 * Ethernet decoder.
 *
 * Useful resources:
 *
 * + http://en.wikipedia.org/wiki/Ethernet_frame
 * + http://www.dcs.gla.ac.uk/~lewis/networkpages/m04s03EthernetFrame.htm
 * + http://standards.ieee.org/about/get/802/802.3.html
 * + http://blog.globalknowledge.com/technology/what-is-the-difference-between-ethernet-ii-and-ieee-802-3/
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
        frame = decode(data);
        if (stringify) {
          frame = JSON.stringify(frame);
        }
      } catch (err) {
        err.data = data; // Attach raw bytes to error.
        this.emit('invalid', err);
        return callback();
      }
      return callback(null, frame);

    };

  }

  function decode(buf) {

    var frame = {};

    frame.da = decodeMacAddr(buf, 0);
    frame.sa = decodeMacAddr(buf, 6);

    var type = 256 * buf[12] + buf[13]; // Big endian.
    if (type <= 0x05DC) {
      frame.length = type;
    } else {
      switch (type) {
        case 0x0800:
          frame.type = 'ipv4';
          break;
        case 0x0806:
          frame.type = 'arp';
          break;
        case 0x8035:
          frame.type = 'rarp';
          break;
        case 0x814c:
          frame.type = 'snmp';
          break;
        case 0x86dd:
          frame.type = 'ipv6';
          break;
        case 0x88cc:
          frame.type = 'lldp';
          break;
        default:
          frame.type = 'unknown';
          break;
      }
    }

    frame.data = new Buffer(buf.length - 18); // TODO: handle padding case.
    buf.copy(frame.data, 0, 14, buf.length - 4);

    return frame;

  }

  // TODO make this more efficient?
  function lpad(str, len) {

    while (str.length < len) {
      str = '0' + str;
    }
    return str;

  }

  function decodeMacAddr(buf, offset) {

    return [
      lpad(buf[offset].toString(16), 2),
      lpad(buf[offset + 1].toString(16), 2),
      lpad(buf[offset + 2].toString(16), 2),
      lpad(buf[offset + 3].toString(16), 2),
      lpad(buf[offset + 4].toString(16), 2),
      lpad(buf[offset + 5].toString(16), 2)
    ].join(':');

  }

  root.exports = {
    Transform: Transform,
    decode: decode
  };

})(module);
