/* jshint node: true */

(function (root) {
  'use strict';

  var stream = require('stream'),
      util = require('util'),
      addon = require('./utils').requireAddon();

  // PDU conversion utilities.

  // https://github.com/mfontanini/libtins/blob/master/include/tins/pdu.h
  var pdus = [
    'RAW',
    'ETHERNET_II',
    'IEEE802_3',
    'RADIOTAP',
    'DOT11',
    'DOT11_ACK',
    'DOT11_ASSOC_REQ',
    'DOT11_ASSOC_RESP',
    'DOT11_AUTH',
    'DOT11_BEACON',
    'DOT11_BLOCK_ACK',
    'DOT11_BLOCK_ACK_REQ',
    'DOT11_CF_END',
    'DOT11_DATA',
    'DOT11_CONTROL',
    'DOT11_DEAUTH',
    'DOT11_DIASSOC',
    'DOT11_END_CF_ACK',
    'DOT11_MANAGEMENT',
    'DOT11_PROBE_REQ',
    'DOT11_PROBE_RESP',
    'DOT11_PS_POLL',
    'DOT11_REASSOC_REQ',
    'DOT11_REASSOC_RESP',
    'DOT11_RTS',
    'DOT11_QOS_DATA',
    'LLC',
    'SNAP',
    'IP',
    'ARP',
    'TCP',
    'UDP',
    'ICMP',
    'BOOTP',
    'DHCP',
    'EAPOL',
    'RC4EAPOL',
    'RSNEAPOL',
    'DNS',
    'LOOPBACK',
    'IPv6',
    'ICMPv6',
    'SLL',
    'DHCPv6',
    'DOT1Q',
    'PPPOE',
    'STP',
    'PPI',
    'IPSEC_AH',
    'IPSEC_ESP',
    'PKTAP'
  ];

  var pduTypes = (function () {

    var types = {};
    var i;
    for (i = 0; i < pdus.length; i++) {
      types[pdus[i]] = i;
    }
    return types;

  })();

  /**
   * Get a name from a PDU type.
   *
   */
  function getPduName(pduType) {

    return pdus[pduType];

  }

  /**
   * A frame, magic!
   *
   */
  var Frame = addon.Frame;

  Frame.prototype.inspect = function () {

    var pduNames = getPduNames(this);

    if (pduNames.length) {
      return '<Frame ' + pduNames.join(' | ') + '>';
    } else {
      // Invalid frame.
      return '<Frame !>';
    }

  };

  function getPduNames(frame) {

    var pduNames = [];
    var pduTypes = frame.getPduTypes();
    var l = pduTypes.length;
    var i;
    if (l > 0) {
      // Valid frame.
      for (i = 0; i < l; i++) {
        pduNames.push(getPduName(pduTypes[i]) || '?');
      }
    }
    return pduNames;

  }

  /**
   * Decoder stream class.
   *
   * This class will "dynamically" look up the correct transform function based
   * on the link type of the source stream. This is done by having a
   * placeholder `_transform` method replace itself when it is first called.
   *
   */
  function Decoder(opts) {

    opts = opts || {};
    var linkType = opts.linkType || null; // Inferred below.

    stream.Transform.call(this, {objectMode: true});

    this.on('pipe', function (src) {
      // Infer link type from first capture stream piped.

      if (!linkType) {
        linkType = src.getLinkType();
      } else if (linkType !== src.getLinkType()) {
        return this.emit('error', new Error('Inconsistent link type.'));
      }

    });

    this.getLinkType = function () { return linkType; };

    this._transform = function (data, encoding, callback) {

      activate(this);
      return this._transform(data, encoding, callback);

    };

    function activate(self) {

      if (!linkType) {
        throw new Error('No link type specified.');
      }

      self._transform = function (data, encoding, callback) {

        var frame;
        try {
          frame = new addon.Frame(linkType, data);
        } catch (err) {
          err.data = data;
          return callback(err);
        }
        if (frame.isValid()) {
          callback(null, frame);
        } else {
          this.emit('invalid', frame);
          callback();
        }

      };

    }

  }
  util.inherits(Decoder, stream.Transform);

  root.exports = {
    Decoder: Decoder,
    Frame: Frame,
    getPduName: getPduName,
    pduTypes: pduTypes
  };

})(module);
