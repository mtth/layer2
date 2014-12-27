/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var assert = require('assert'),
      dot11 = require('../');

  var captures = {
    small: {
      path: './test/dat/mesh3.pcap',
      length: 3
    },
    large: {
      path: './test/dat/mesh780.pcap',
      length: 780
    }
  };

  /**
   * The sample file used has the following packets:
   *
   * + Invalid (version 3 802.11)
   * + Valid
   *
   */
  describe('Decoder', function () {

    var Decoder = dot11.transform.Decoder;

    function validate(datalink, hexBytes, expectedObject) {

      var buf = new Buffer(hexBytes, 'hex');
      var actualObject = Decoder[datalink](buf);
      assert.deepEqual(
        actualObject,
        expectedObject,
        JSON.stringify(actualObject) + ' != ' + JSON.stringify(expectedObject)
      );

    }

    it('decodes radiotap frames', function () {

      validate(
        'IEEE802_11_RADIO',
        '000020006708040054c6b82400000000220cdaa002000000400100003c142411aa',
        {
          'headerRevision': 0,
          'headerPad': 0,
          'headerLength': 32,
          'body': new Buffer('aa', 'hex')
        }
      );

    });

    it('decodes 802.11 beacon frames', function () {

      validate(
        'IEEE802_11_FRAME',
        '80000000ffffffffffff06037f07a01606037f07a016b077',
        {
          'version': 0,
          'type': 'mgmt',
          'subType': 'beacon',
          'toDs': 0,
          'fromDs': 0,
          'duration': 0,
          'ra': 'ff:ff:ff:ff:ff:ff',
          'ta': '06:03:7f:07:a0:16',
          'da': 'ff:ff:ff:ff:ff:ff',
          'sa': '06:03:7f:07:a0:16',
          'bssid': '06:03:7f:07:a0:16'
        }
      );

    });

    it('decodes 802.11 rts frames', function () {

      validate(
        'IEEE802_11_FRAME',
        'b400c400606c668ff5e3ac220bce6de0',
        {
          'version': 0,
          'type': 'ctrl',
          'subType': 'rts',
          'toDs': 0,
          'fromDs': 0,
          'duration': 196,
          'ra': '60:6c:66:8f:f5:e3',
          'ta': 'ac:22:0b:ce:6d:e0'
        }
      );

    });

    it('decodes 802.11 cts frames', function () {

      validate(
        'IEEE802_11_FRAME',
        'c400da0f606c668ff5e3',
        {
          'version': 0,
          'type': 'ctrl',
          'subType': 'cts',
          'toDs': 0,
          'fromDs': 0,
          'duration': 4058,
          'ra': '60:6c:66:8f:f5:e3'
        }
      );

    });

    it('decodes 802.11 data frames', function () {

      validate(
        'IEEE802_11_FRAME',
        '08420000ffffffffffffac220bce6de0ac220bce6de0a0e45c2400a000000000',
        {
          'version': 0,
          'type': 'data',
          'subType': 'data',
          'toDs': 0,
          'fromDs': 1,
          'duration': 0,
          'ra': 'ff:ff:ff:ff:ff:ff',
          'ta': 'ac:22:0b:ce:6d:e0',
          'da': 'ff:ff:ff:ff:ff:ff',
          'bssid': 'ac:22:0b:ce:6d:e0' // No SA.
        }
      );

    });

    it('decodes 802.11 qos frames', function () {

      validate(
        'IEEE802_11_FRAME',
        'c8093c0016abf0a58460fc4dd42bab2816abf0a5846040b70000',
        {
          'version': 0,
          'type': 'data',
          'subType': 'qos',
          'toDs': 1,
          'fromDs': 0,
          'duration': 60,
          'ra': '16:ab:f0:a5:84:60',
          'ta': 'fc:4d:d4:2b:ab:28',
          'sa': 'fc:4d:d4:2b:ab:28',
          'bssid': '16:ab:f0:a5:84:60' // No DA.
        }
      );

    });

    it('decodes 802.11 ack frames', function () {

      validate(
        'IEEE802_11_FRAME',
        'd4000000606c668ff5e3',
        {
          'version': 0,
          'type': 'ctrl',
          'subType': 'ack',
          'toDs': 0,
          'fromDs': 0,
          'duration': 0,
          'ra': '60:6c:66:8f:f5:e3'
        }
      );

    });

    // Streaming check.

    it('can be piped to and read from', function (done) {

      var capture = new dot11.capture.Replay(captures.small.path);
      var decoder = new Decoder({linkType: capture.getLinkType()});

      capture
        .pipe(decoder)
        .once('readable', function () {
          assert.equal(this.getLinkType(), 'IEEE802_11_RADIO');
          var packet = this.read();
          assert.deepEqual(packet, {
            'headerRevision': 0,
            'headerPad': 0,
            'headerLength': 32,
            'body': new Buffer('80000000ffffffffffff06037f07a01606037f07a016b0773a40cb260000000064000105000a667265656273642d617001088c129824b048606c030124050400010000072a5553202401112801112c01113001113401173801173c011740011795011e99011e9d011ea1011ea5011e200100dd180050f2020101000003a4000027a4000042435e0062322f00', 'hex')
          });
          done();
        });

    });

    it('can be piped to and listened to', function (done) {

      var capture = new dot11.capture.Replay(captures.small.path);
      var decoder = new Decoder();
      var nPackets = 0;

      capture
        .pipe(decoder)
        .on('data', function () { nPackets++; })
        .on('end', function () {
          assert.equal(this.getLinkType(), 'IEEE802_11_RADIO');
          assert.equal(nPackets, captures.small.length);
          done();
        });

    });

  });

  describe('Extractor', function () {

    var Extractor = dot11.transform.Extractor;

    it('infers the from type', function (done) {

      var capture = new dot11.capture.Replay(captures.small.path);
      var extractor = new Extractor({toLinkType: 'IEEE802_11_FRAME'});

      capture
        .pipe(extractor)
        .once('readable', function () {
          assert.equal(this.getLinkType(true), 'IEEE802_11_RADIO');
          assert.equal(this.getLinkType(), 'IEEE802_11_FRAME');
          done();
        });

    });

    it('infers the from and to types when possible', function (done) {

      var capture = new dot11.capture.Replay(captures.small.path);
      var extractor = new Extractor();

      capture
        .pipe(extractor)
        .once('readable', function () {
          assert.equal(this.getLinkType(true), 'IEEE802_11_RADIO');
          assert.equal(this.getLinkType(), 'IEEE802_11_FRAME');
          done();
        });

    });

    it('extracts all valid packets', function (done) {

      var capture = new dot11.capture.Replay(captures.large.path);
      var extractor = new Extractor({toLinkType: 'IEEE802_11_FRAME'});
      var nPackets = 0;

      capture
        .pipe(extractor)
        .on('data', function () { nPackets++; })
        .on('end', function () {
          assert.equal(nPackets, captures.large.length);
          done();
        });

    });

    it('extracts IEEE802_11_RADIO to IEEE802_11_FRAME', function (done) {

      var capture = new dot11.capture.Replay(captures.small.path);
      var extractor = new Extractor({
        fromLinkType: 'IEEE802_11_RADIO',
        toLinkType: 'IEEE802_11_FRAME'
      });
      var decoder = new dot11.transform.Decoder();

      capture
        .pipe(extractor)
        .pipe(decoder)
        .once('readable', function () {
          assert.equal(this.getLinkType(), 'IEEE802_11_FRAME');
          assert.deepEqual(this.read() , {
            version: 0,
            type: 'mgmt',
            subType: 'beacon',
            toDs: 0,
            fromDs: 0,
            duration: 0,
            ra: 'ff:ff:ff:ff:ff:ff',
            da: 'ff:ff:ff:ff:ff:ff',
            ta: '06:03:7f:07:a0:16',
            sa: '06:03:7f:07:a0:16',
            bssid: '06:03:7f:07:a0:16'
          });
          done();
        });

    });

  });

})();
