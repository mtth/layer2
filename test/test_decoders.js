/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var assert = require('assert'),
      diff = require('deep-diff'),
      decoders = require('../lib/decoders'), // Not exposed via dot11.
      dot11 = require('../lib');

  describe('Decoder for', function () {

    describe('IEEE802_11_RADIO', function () {

      var validate = _validate(decoders.IEEE802_11_RADIO);
      var hexPackets = [
        '000020006708040054c6b82400000000220cdaa002000000400100003c142411aa'
      ];

      it('decodes radiotap frames', function () {

        validate(
          hexPackets[0],
          {
            'headerRevision': 0,
            'headerPad': 0,
            'headerLength': 32,
            'body': new Buffer('aa', 'hex')
          }
        );

      });

    });

    describe('IEEE802_11', function () {

      var validate = _validate(decoders.IEEE802_11);
      var hexPackets = [
        '80000000ffffffffffff06037f07a01606037f07a016b077', // beacon
        'b400c400606c668ff5e3ac220bce6de0', // rts
        'c400da0f606c668ff5e3', // cts
        '08420000ffffffffffffac220bce6de0ac220bce6de0a0e45c2400a000000000', // data
        'c8093c0016abf0a58460fc4dd42bab2816abf0a5846040b70000', // qos
        'd4000000606c668ff5e3', // ack
        '940000008438355f8e8a08863b3b39c70510a0a40100000000000000ed91e950', // block-ack
        '98bffe15c47e5e58d84dcedea94f2bc7da666bd6739e6bd8803bfde0fffd8cb7', // qos-data cf-ack
      ];

      it('decodes beacon frames', function () {

        validate(
          hexPackets[0],
          {
            'version': 0,
            'type': 'mgmt',
            'subType': 'beacon',
            'toDs': 0,
            'fromDs': 0,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'duration': 0,
            'ra': 'ff:ff:ff:ff:ff:ff',
            'ta': '06:03:7f:07:a0:16',
            'da': 'ff:ff:ff:ff:ff:ff',
            'sa': '06:03:7f:07:a0:16',
            'bssid': '06:03:7f:07:a0:16'
          }
        );

      });

      it('decodes rts frames', function () {

        validate(
          hexPackets[1],
          {
            'version': 0,
            'type': 'ctrl',
            'subType': 'rts',
            'toDs': 0,
            'fromDs': 0,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'duration': 196,
            'ra': '60:6c:66:8f:f5:e3',
            'ta': 'ac:22:0b:ce:6d:e0'
          }
        );

      });

      it('decodes cts frames', function () {

        validate(
          hexPackets[2],
          {
            'version': 0,
            'type': 'ctrl',
            'subType': 'cts',
            'toDs': 0,
            'fromDs': 0,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'duration': 4058,
            'ra': '60:6c:66:8f:f5:e3'
          }
        );

      });

      it('decodes data frames', function () {

        validate(
          hexPackets[3],
          {
            'version': 0,
            'type': 'data',
            'subType': 'data',
            'toDs': 0,
            'fromDs': 1,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'fragN': 0,
            'seqN': 3658,
            'duration': 0,
            'ra': 'ff:ff:ff:ff:ff:ff',
            'ta': 'ac:22:0b:ce:6d:e0',
            'da': 'ff:ff:ff:ff:ff:ff',
            'sa': 'ac:22:0b:ce:6d:e0',
            'bssid': 'ac:22:0b:ce:6d:e0',
            'encryption': 'ccmp',
            'body': new Buffer(0) // FIXME
          }
        );

      });

      it('decodes qos frames', function () {

        validate(
          hexPackets[4],
          {
            'version': 0,
            'type': 'data',
            'subType': 'qos',
            'toDs': 1,
            'fromDs': 0,
            'retry': 1,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'fragN': 0,
            'seqN': 2932,
            'duration': 60,
            'ra': '16:ab:f0:a5:84:60',
            'ta': 'fc:4d:d4:2b:ab:28',
            'da': '16:ab:f0:a5:84:60',
            'sa': 'fc:4d:d4:2b:ab:28',
            'bssid': '16:ab:f0:a5:84:60'
          }
        );

      });

      it('decodes ack frames', function () {

        validate(
          hexPackets[5],
          {
            'version': 0,
            'type': 'ctrl',
            'subType': 'ack',
            'toDs': 0,
            'fromDs': 0,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'duration': 0,
            'ra': '60:6c:66:8f:f5:e3'
          }
        );

      });

      it('decodes block-ack frames', function () {

        validate(
          hexPackets[6],
          {
            'version': 0,
            'type': 'ctrl',
            'subType': 'block-ack',
            'toDs': 0,
            'fromDs': 0,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'duration': 0,
            'ra': '84:38:35:5f:8e:8a',
            'ta': '08:86:3b:3b:39:c7'
          }
        );

      });

      it.skip('decodes qos-data cf-ack frames', function () {

        validate(
          hexPackets[7],
          {
            'version': 0,
            'type': 'data',
            'subType': 'qos-data cf-ack',
            'toDs': 1,
            'fromDs': 1,
            'retry': 1,
            'powerMgmt': 1,
            'moreFrag': 1,
            'moreData': 1,
            'fragN': 11,
            'seqN': 3462,
            'duration': 0,
            'ra': 'c4:7e:5e:58:d8:4d',
            'ta': 'ce:de:a9:4f:2b:c7',
            'da': 'da:66:6b:d6:73:9e',
            'sa': '80:3b:fd:e0:ff:fd'
          }
        );

      });

      // _save('frame.pcap', 'IEEE802_11', hexPackets);

    });

  });

  // Create decoder specific validator.
  function _validate(decoder) {

    return function(hexBytes, expectedObject) {

      var buf = new Buffer(hexBytes, 'hex');
      var actualObject = decoder.decode(buf);
      assert.deepEqual(
        actualObject,
        expectedObject,
        JSON.stringify(diff(actualObject, expectedObject), null, 2)
      );

    };

  }

  // Save list of buffers to a file to be compared with Wireshark.
  function _save(path, linkType, hexPackets) { // jshint ignore:line

    var save = new dot11.capture.Save(path, {linkType: linkType});

    var i;
    for (i = 0; i < hexPackets.length; i++) {
      save.write(new Buffer(hexPackets[i], 'hex'));
    }
    save.end();

  }

})();
