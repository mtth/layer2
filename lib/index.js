/* jshint node: true */

(function (root) {
  'use strict';

  var addon = require('./utils').requireAddon();

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

  root.exports = {
    capture: require('./capture'),
    Frame: Frame,
    getPduName: getPduName,
    pduTypes: pduTypes
  };

})(module);
