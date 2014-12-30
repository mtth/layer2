/* jshint node: true */

/**
 * Useful resources:
 *
 * + http://standards.ieee.org/about/get/802/802.11.html
 *
 */
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
        // Not emitting errors here because node would unpipe any readable
        // stream writing to this decoder when it emits an error (and it is
        // expensive to reattach them each time). Maybe add a strict mode to
        // convert all these warnings into errors but this is hard to control
        // since there are corrupted packets relatively often.
        return callback();
      }
      return callback(null, packet);

    };

  }

  function decode(buf) {

    var frame = {};

    var frameControl = asUint16Le(buf, 0);
    frame.version = frameControl & 0x03;
    switch ((frameControl << 2) & 0x30 | (frameControl >> 4) & 0x0f) {
      // Just put type frames bits "above" subtype bits, onward!
      // Management frames.
      case 0:
        frame.type = 'mgmt';
        frame.subType = 'assoc-req';
        break;
      case 1:
        frame.type = 'mgmt';
        frame.subType = 'assoc-res';
        break;
      case 2:
        frame.type = 'mgmt';
        frame.subType = 'reassoc-req';
        break;
      case 3:
        frame.type = 'mgmt';
        frame.subType = 'reassoc-res';
        break;
      case 4:
        frame.type = 'mgmt';
        frame.subType = 'probe-req';
        break;
      case 5:
        frame.type = 'mgmt';
        frame.subType = 'probe-res';
        break;
      case 6:
        frame.type = 'mgmt';
        frame.subType = 'timing-avd';
        break;
      case 7:
        frame.type = 'mgmt';
        frame.subType = 'rsvd';
        break;
      case 8:
        frame.type = 'mgmt';
        frame.subType = 'beacon';
        break;
      case 9:
        frame.type = 'mgmt';
        frame.subType = 'atim';
        break;
      case 10:
        frame.type = 'mgmt';
        frame.subType = 'diassoc';
        break;
      case 11:
        frame.type = 'mgmt';
        frame.subType = 'auth';
        break;
      case 12:
        frame.type = 'mgmt';
        frame.subType = 'deauth';
        break;
      case 13:
        frame.type = 'mgmt';
        frame.subType = 'action';
        break;
      case 14:
        frame.type = 'mgmt';
        frame.subType = 'action-no-ack';
        break;
      case 15:
        frame.type = 'mgmt';
        frame.subType = 'rsvd';
        break;
      // Control frames.
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
        frame.type = 'ctrl';
        frame.subType = 'rsvd';
        break;
      case 23:
        frame.type = 'ctrl';
        frame.subType = 'ctrl-wrapper';
        break;
      case 24:
        frame.type = 'ctrl';
        frame.subType = 'block-ack-req';
        break;
      case 25:
        frame.type = 'ctrl';
        frame.subType = 'block-ack';
        break;
      case 26:
        frame.type = 'ctrl';
        frame.subType = 'ps-poll';
        break;
      case 27:
        frame.type = 'ctrl';
        frame.subType = 'rts';
        break;
      case 28:
        frame.type = 'ctrl';
        frame.subType = 'cts';
        break;
      case 29:
        frame.type = 'ctrl';
        frame.subType = 'ack';
        break;
      case 30:
        frame.type = 'ctrl';
        frame.subType = 'cf-end';
        break;
      case 31:
        frame.type = 'ctrl';
        frame.subType = 'cf-end cf-ack';
        break;
      // Data frames.
      case 32:
        frame.type = 'data';
        frame.subType = 'data';
        break;
      case 33:
        frame.type = 'data';
        frame.subType = 'data cf-ack';
        break;
      case 34:
        frame.type = 'data';
        frame.subType = 'data cf-poll';
        break;
      case 35:
        frame.type = 'data';
        frame.subType = 'data cf-ack cf-poll';
        break;
      case 36:
        frame.type = 'data';
        frame.subType = 'null';
        break;
      case 37:
        frame.type = 'data';
        frame.subType = 'cf-ack';
        break;
      case 38:
        frame.type = 'data';
        frame.subType = 'cf-poll';
        break;
      case 39:
        frame.type = 'data';
        frame.subType = 'cf-ack cf-poll';
        break;
      case 40:
        frame.type = 'data';
        frame.subType = 'qos-data';
        break;
      case 41:
        frame.type = 'data';
        frame.subType = 'qos-data cf-ack';
        break;
      case 42:
        frame.type = 'data';
        frame.subType = 'qos-data cf-poll';
        break;
      case 43:
        frame.type = 'data';
        frame.subType = 'qos-data cf-ack cf-poll';
        break;
      case 44:
        frame.type = 'data';
        frame.subType = 'qos';
        break;
      case 45:
        frame.type = 'data';
        frame.subType = 'rsvd';
        break;
      case 46:
        frame.type = 'data';
        frame.subType = 'qos cf-poll';
        break;
      case 47:
        frame.type = 'data';
        frame.subType = 'qos cf-ack cf-poll';
        break;
      // Reserved (anything over 48).
      default:
        frame.type = 'rsvd';
        frame.subType = 'rsvd';
    }

    var flags = (frameControl >> 8) & 0xff;
    frame.toDs = flags & 0x01;
    frame.fromDs = (flags >> 1) & 0x01;
    frame.moreFrag = (flags >> 2) & 0x01;
    frame.retry = (flags >> 3) & 0x01;
    frame.powerMgmt = (flags >> 4) & 0x01;
    frame.moreData = (flags >> 5) & 0x01;
    var isProtected = (flags >> 6) & 0x01; // Transformed to `encryption` below.
    var isOrdered = (flags >> 7) & 0x01;

    var duration = asUint16Le(buf, 2); // Can be duration, AID, or nothing.
    if ((duration >> 15) & 0x01) {
      if (frame.subType === 'ps-poll') {
        frame.aid = duration;
      }
    } else {
      frame.duration = duration;
    }

    // Rest of packet is mostly type specific.
    switch (frame.type) {

      case 'ctrl':
        frame.ra = decodeMacAddr(buf, 4);
        switch (frame.subType) {
          case 'rts':
          case 'block-ack':
          case 'block-ack-req':
            frame.ta = decodeMacAddr(buf, 10);
            break;
          case 'ps-poll':
            frame.bssid = frame.ra;
            frame.ta = decodeMacAddr(buf, 10);
            break;
          case 'cf-end':
          case 'cf-end cf-ack':
            frame.ta = frame.bssid = decodeMacAddr(buf, 10);
            break;
          case 'ctrl-wrapper':
            var carriedBuf = new Buffer(buf.length - 10);
            buf.copy(carriedBuf, 0, 10, 12); // Copy frame control.
            buf.copy(carriedBuf, 2, 2, 10); // Copy duration and address 1.
            buf.copy(carriedBuf, 10, 16, buf.length - 4); // Copy rest of frame.
            frame.carriedFrame = decode(carriedBuf); // TODO: check that this works.
            break;
          default:
            // 'cts' 'ack': nothing to do.
        }
        break;

      case 'data':
        var hasAddress4 = !(~flags & 0x03);
        var hasQosCtrl = (frameControl >> 7) & 0x01;
        var hasHtCtrl = hasQosCtrl && isOrdered;
        var hasData = ~(frameControl >> 6) & 0x01;

        // First two addresses are easy.
        frame.ra = decodeMacAddr(buf, 4);
        frame.ta = decodeMacAddr(buf, 10);

        // Sequence control things.
        var seqControl = asUint16Le(buf, 22);
        frame.fragN = seqControl & 0x07;
        frame.seqN = seqControl >> 4;

        // Find out if we are dealing with A-MSDU or MSDU (for addresses 3/4)
        // then fill in the remaining, trickier, addresses.
        var hasAmsdu = hasQosCtrl & hasData &
          (buf[hasAddress4 ? 30 : 24] >> 7 & 0x01); // Look into Qos Ctrl field.
        switch (flags & 0x03) {
          case 0:
            frame.da = frame.ra;
            frame.sa = frame.ta;
            frame.bssid = decodeMacAddr(buf, 16);
            break;
          case 1: // toDs
            frame.bssid = frame.ra;
            frame.sa = frame.ta;
            if (hasAmsdu) {
              frame.bssid = decodeMacAddr(buf, 16);
            } else {
              frame.da = decodeMacAddr(buf, 16);
            }
            break;
          case 2: // fromDs
            frame.da = frame.ra;
            frame.bssid = frame.ta;
            if (hasAmsdu) {
              frame.bssid = decodeMacAddr(buf, 16);
            } else {
              frame.sa = decodeMacAddr(buf, 16);
            }
            break;
          default: // fromDs toDs
            if (hasAmsdu) {
              frame.bssid = decodeMacAddr(buf, 16);
            } else {
              frame.da = decodeMacAddr(buf, 16);
              frame.sa = decodeMacAddr(buf, 24);
            }
        }
        // TODO: Implement A-MSDU parsing.

        // If this frame has data, attach it.
        if (hasData) {
          // Offset where encryption starts (if any).
          var encryptionOffset = 24 +
            (hasAddress4 ? 6 : 0) +
            (hasQosCtrl ? 2 : 0) +
            (hasHtCtrl ? 4 : 0);
          var dataOffset;
          if (!isProtected) {
            dataOffset = encryptionOffset;
            frame.encryption = null;
          } else {
            // http://wifidot11.blogspot.com/2010/09/wireshark-makes-mistake-ccmp-encrypted.html
            if ((frame[encryptionOffset + 3] >> 5) & 0x01) {
              // WEP.
              dataOffset = encryptionOffset + 4;
              frame.encryption = 'wep';
            } else {
              dataOffset = encryptionOffset + 8;
              frame.encryption = frame[encryptionOffset + 2] ? 'tkip' : 'ccmp';
              // Recall that TKIP <=> WAP and CCMP <=> WAP2.
              // Note also that this has a small chance of being wrong (see
              // article above).
            }
          }
          frame.data = new Buffer(buf.length - 4 - dataOffset);
          buf.copy(frame.data, 0, dataOffset, buf.length - 4);
        }
        break;

      case 'mgmt':
        frame.da = frame.ra = decodeMacAddr(buf, 4);
        frame.sa = frame.ta = decodeMacAddr(buf, 10);
        frame.bssid = decodeMacAddr(buf, 16);
        // TODO: finish management frames handling.
        break;

      default:
        // No other type should be here.

    }

    return frame;

  }

  function asUint16Le(buf, offset) {

    return buf[offset+1] * 256 + buf[offset];

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
