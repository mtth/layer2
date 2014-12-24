/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var assert = require('assert'),
      dot11 = require('../');

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
      assert.deepEqual(Decoder[datalink](buf, 0, true), expectedObject);

    }

    it('decodes radiotap frames', function () {

      validate(
        'IEEE802_11_RADIO',
        '000020006708040054c6b82400000000220cdaa002000000400100003c142411aa',
        {
          'headerRevision': 0,
          'headerPad': 0,
          'headerLength': 32,
          'body': 'aa'
        }
      );

    });

    it('decodes 802.11 beacon frames', function () {

      validate(
        'IEEE802_11_FRAME',
        '80000000ffffffffffff06037f07a01606037f07a016b077',
        {
          'version': 0,
          'type':'mgmt',
          'subType':'beacon',
          'toDs':0,
          'fromDs':0,
          'duration':0,
          'ra':'ff:ff:ff:ff:ff:ff',
          'ta':'06:03:7f:07:a0:16',
          'da':'ff:ff:ff:ff:ff:ff',
          'sa':'06:03:7f:07:a0:16',
          'bssid':'06:03:7f:07:a0:16'
        }
      );

    });

    // Streaming check.

    var sampleCapturePath = './test/dat/mesh3.pcap';

    it('can be piped to', function (done) {

      var capture = new dot11.capture.Replay(sampleCapturePath);
      var decoder = new Decoder('IEEE802_11_RADIO');
      var nPackets = 0;

      capture
        .pipe(decoder)
        .on('data', function () { nPackets++; })
        .on('end', function () {
          assert.equal(nPackets, 3);
          done();
        });

    });

    it('can be read from', function (done) {

      var capture = new dot11.capture.Replay(sampleCapturePath);
      var decoder = new Decoder('IEEE802_11_RADIO');

      capture
        .pipe(decoder)
        .once('readable', function () {
          var packet = this.read();
          assert.deepEqual(packet, {
            'headerRevision': 0,
            'headerPad': 0,
            'headerLength': 32,
            'body': {
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
          });
          done();
        });

    });


  });



})();
