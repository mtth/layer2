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

  var utils = require('../utils');

  function decode(buf) {

    // Validate checksum.
    var actualFcs = buf.readUInt32LE(buf.length - 4);
    var computedFcs = utils.crc32(buf.slice(0, buf.length - 4));
    if (actualFcs !== computedFcs) {
      throw new Error('Invalid FCS.');
    }

    // Parse addresses.
    var frame = {};
    frame.da = utils.readMacAddr(buf, 0);
    frame.sa = utils.readMacAddr(buf, 6);

    // Add type.
    var type = buf.readUInt16BE(12);
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
          // TODO: add more Ethernet types.
      }
    }

    frame.body = new Buffer(buf.length - 18);
    // TODO: handle padding case when the length is small.
    buf.copy(frame.body, 0, 14, buf.length - 4);

    return frame;

  }

  root.exports = decode;

})(module);
