/* jshint node: true */

/**
 * Packet decoding functions and associated streams.
 *
 */
(function (root) {
  'use strict';

  var stream = require('stream'),
      util = require('util');

  /**
   * Decoder stream class.
   *
   */
  function Decoder(opts) {

    opts = opts || {};
    var ignoreErrors = opts.ignoreErrors || false;
    var linkType = opts.linkType; // Inferred below.
    var stringify = opts.stringify || false;

    stream.Transform.call(this, {objectMode: true});

    var decode = null;

    this.on('pipe', function (src) {
      // Infer link type from first capture stream piped.

      if (!linkType) {
        linkType = src.getLinkType();
      } else if (linkType !== src.getLinkType()) {
        return this.emit('error', new Error('Inconsistent link type.'));
      }

    });

    this._transform = function (data, encoding, callback) {

      if (!decode) {
        if (!linkType) {
          return this.emit('error', new Error('No link type specified.'));
        } else if (!(linkType in Decoder)) {
          return this.emit('error', new Error('Unsupported link type.'));
        }
        decode = Decoder[linkType];
      }

      var packet;
      try {
        packet = decode(data);
        if (stringify) {
          packet = JSON.stringify(packet);
        }
      } catch (err) {
        return ignoreErrors ? callback() : callback(err);
      }
      callback(null, packet);

    };

    this.getLinkType = function () { return linkType; };

  }
  util.inherits(Decoder, stream.Transform);


  // Decoder implementations.

  /**
   * Decode radiotap frame.
   *
   */
  Decoder.IEEE802_11_RADIO = function (buf, offset) {

    offset = offset || 0;

    var frame = {};
    frame.headerRevision = buf[offset];
    frame.headerPad = buf[offset + 1];
    frame.headerLength = unpack.uint16_be(buf, offset + 2);
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

  };

  Decoder.IEEE802_11_FRAME = function (buf, offset) {

    offset = offset || 0;

    var frame = {};

    var frameControl = unpack.uint16_be(buf, offset);
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
    // TODO: Also add other flags?

    frame.duration = unpack.uint16_be(buf, offset + 2);

    // Addresses.
    switch (frame.type) {
    case 'mgmt':
      frame.da = frame.ra = unpack.macAddr(buf, offset + 4);
      frame.sa = frame.ta = unpack.macAddr(buf, offset + 10);
      frame.bssid = unpack.macAddr(buf, offset + 16);
      break;
    case 'ctrl':
      switch (frame.subType) {
      case 'rts':
        frame.ra = unpack.macAddr(buf, offset + 4);
        frame.ta = unpack.macAddr(buf, offset + 10);
        break;
      case 'cts':
      case 'ack':
        frame.ra = unpack.macAddr(buf, offset + 4);
        break;
      case 'cts':
        frame.ra = unpack.macAddr(buf, offset + 4);
        break;
      case 'ps-poll':
        frame.ra = frame.bssid = unpack.macAddr(buf, offset + 4);
        frame.ta = unpack.macAddr(buf, offset + 10);
        break;
      case 'cf-end':
      case 'cf-end cf-ack':
        frame.ra = unpack.macAddr(buf, offset + 4);
        frame.ta = frame.bssid = unpack.macAddr(buf, offset + 10);
        break;
      case 'block-ack':
        frame.ra = unpack.macAddr(buf, offset + 4);
        frame.ta = unpack.macAddr(buf, offset + 10);
        break;
      case 'ctrl-wrapper':
        frame.ra = unpack.macAddr(buf, offset + 4);
        frame.data = Decoder.IEEE802_11_FRAME(buf, offset + 16);
        // Note that there is still a trailing FCS after the carried frame.
        break;
      default:
      }
      break;
    case 'data':
      frame.ra = unpack.macAddr(buf, offset + 4);
      frame.ta = unpack.macAddr(buf, offset + 10);
      switch (flags & 0x03) {
        case 0:
          frame.da = frame.ra;
          frame.sa = frame.ta;
          frame.bssid = unpack.macAddr(buf, offset + 16);
          break;
        case 1: // toDs
          frame.bssid = frame.ra;
          frame.sa = frame.ta;
          break;
        case 2: // fromDs
          frame.da = frame.ra;
          frame.bssid = frame.ta;
          break;
        default: // fromDs toDs
      }
      // TODO: figure out MSDU v.s. A-MSDU case to fill out all addresses here.
      break;
    default:
    }

    return frame;

  };

  /**
   * Extractor stream classes.
   *
   */
  function Extractor(opts) {

    opts = opts || {};
    var ignoreErrors = opts.ignoreErrors || false;
    var fromLinkType = opts.fromLinkType;
    var toLinkType = opts.toLinkType;

    stream.Duplex.call(this, {objectMode: true});

    this.activate = function () {

      if (fromLinkType && toLinkType) {
        var srcExtractors = Extractor[fromLinkType];
        if (srcExtractors && toLinkType in srcExtractors) {
          return srcExtractors[toLinkType].call(this, ignoreErrors);
        }
      }
      this.emit('error', new Error('Invalid link types.'));

    };

    this.on('pipe', function (src) {
      // Infer link type from first capture stream piped.

      if (!fromLinkType) {
        fromLinkType = src.getLinkType();
      } else if (fromLinkType !== src.getLinkType()) {
        return this.emit('error', new Error('Inconsistent source link type.'));
      }

      if (!toLinkType && fromLinkType in Extractor) {
        var toLinkTypes = Object.keys(Extractor[fromLinkType]);
        if (toLinkTypes.length === 1) {
          toLinkType = toLinkTypes[0];
        }
      }

      this.activate();

    });

    this.getLinkType = function (incoming) {

      return incoming ? fromLinkType : toLinkType;

    };

  }
  util.inherits(Extractor, stream.Duplex);

  Extractor.IEEE802_11_RADIO = {

    IEEE802_11_FRAME: function (ignoreErrors) {
      // Basic implementation of a sync object stream (i.e. where the
      // `_transform` function is always synchronous).

      var packet = null;
      var needPacket = false;

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

      // Emit 'end' event.
      this.once('finish', function () { this.push(null); });

      /**
       * Extract 802.11 frame from Radiotap packet.
       *
       */
      function extract(buf) {

        var headerLength = unpack.uint16_be(buf, 2);
        var body = new Buffer(buf.length - headerLength);
        buf.copy(body, 0, headerLength);
        return body;

      }

    }

  };

  // Helpers.

  function lpad(str, len) {

    while (str.length < len) {
      str = '0' + str;
    }
    return str;

  }

  var unpack = {

    macAddr: function (raw_packet, offset) {

      return [
        lpad(raw_packet[offset].toString(16), 2),
        lpad(raw_packet[offset + 1].toString(16), 2),
        lpad(raw_packet[offset + 2].toString(16), 2),
        lpad(raw_packet[offset + 3].toString(16), 2),
        lpad(raw_packet[offset + 4].toString(16), 2),
        lpad(raw_packet[offset + 5].toString(16), 2)
      ].join(":");

    },

    sll_addr: function (raw_packet, offset, len) {

      var res = [], i;
      for (i=0; i<len; i++){
        res.push(lpad(raw_packet[offset+i].toString(16), 2));
      }
      return res.join(":");

    },

    uint16: function (raw_packet, offset) {

      return ((raw_packet[offset] * 256) + raw_packet[offset + 1]);

    },

    uint16_be: function (raw_packet, offset) {

      return ((raw_packet[offset+1] * 256) + raw_packet[offset]);

    },

    uint32: function (raw_packet, offset) {

      return (
        (raw_packet[offset] * 16777216) +
        (raw_packet[offset + 1] * 65536) +
        (raw_packet[offset + 2] * 256) +
        raw_packet[offset + 3]
      );

    },

    uint64: function (raw_packet, offset) {

      return (
        (raw_packet[offset] * 72057594037927936) +
        (raw_packet[offset + 1] * 281474976710656) +
        (raw_packet[offset + 2] * 1099511627776) +
        (raw_packet[offset + 3] * 4294967296) +
        (raw_packet[offset + 4] * 16777216) +
        (raw_packet[offset + 5] * 65536) +
        (raw_packet[offset + 6] * 256) +
        raw_packet[offset + 7]
      );

    },

    ipv4_addr: function (raw_packet, offset) {

      return [
        raw_packet[offset],
        raw_packet[offset + 1],
        raw_packet[offset + 2],
        raw_packet[offset + 3]
      ].join('.');

    },

    ipv6_addr: function (raw_packet, offset) {

      var i;
      var ret = '';
      var octets = [];
      for (i=offset; i<offset+16; i+=2) {
      octets.push(unpack.uint16(raw_packet,i).toString(16));
      }
      var curr_start, curr_len;
      var max_start, max_len;
      for(i = 0; i < 8; i++){
        if(octets[i] == "0"){
          if(curr_start === undefined){
            curr_len = 1;
            curr_start = i;
          }else{
            curr_len++;
            if(!max_start || curr_len > max_len){
              max_start = curr_start;
              max_len = curr_len;
            }
          }
        }else{
          curr_start = undefined;
        }
      }
      if(max_start !== undefined){
        var tosplice = max_start === 0 || (max_start + max_len > 7) ? ":" : "";
        octets.splice(max_start, max_len,tosplice);
        if(max_len == 8){octets.push("");}
      }
      ret = octets.join(":");
      return ret;

    }

  };

  root.exports = {
    Decoder: Decoder,
    Extractor: Extractor
  };

})(module);
