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
    },
    mixed: { // With invalid packets.
      path: './test/dat/mixed.pcap',
      length: {valid: 155, invalid: 11}
    }
  };

  /**
   * The sample file used has the following packets:
   *
   * + Invalid (version 3 802.11)
   * + Valid
   *
   */
  describe('Decoder transform', function () {

    var Decoder = dot11.transform.Decoder;

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
        .on('data', function (data) {
          assert.ok(data && typeof data == 'object');
          nPackets++;
        })
        .on('end', function () {
          assert.equal(this.getLinkType(), 'IEEE802_11_RADIO');
          assert.equal(nPackets, captures.small.length);
          done();
        });

    });

    it('emits end when the writable side finished', function (done) {

      var capture = new dot11.capture.Replay(captures.large.path);
      var decoder = new Decoder({linkType: capture.getLinkType()});
      var nPackets = 0;

      capture
        .on('data', function (buf) { decoder.write(buf); })
        .on('end', function () { decoder.end(); });

      decoder
        .on('data', function () { nPackets++; })
        .on('end', function () {
          assert.equal(nPackets, captures.large.length);
          done();
        });

    });

    it('emits events when a packet fails to decode', function (done) {

      var capture = new dot11.capture.Replay(captures.mixed.path);
      var decoder = new Decoder();
      var nValidPackets = 0;
      var nInvalidPackets = 0;

      capture
        .pipe(decoder)
        .on('data', function (data) {
          assert.ok(data && typeof data == 'object');
          nValidPackets++;
        })
        .on('invalid', function (err) {
          assert.ok(err.data && typeof err.data == 'object');
          nInvalidPackets++;
        })
        .on('end', function () {
          assert.equal(nValidPackets, captures.mixed.length.valid);
          assert.equal(nInvalidPackets, captures.mixed.length.invalid);
          done();
        });

    });

  });

  describe('Extractor transform', function () {

    var Extractor = dot11.transform.Extractor;

    it('infers the from type', function (done) {

      var capture = new dot11.capture.Replay(captures.small.path);
      var extractor = new Extractor({toLinkType: 'IEEE802_11'});

      capture
        .pipe(extractor)
        .once('readable', function () {
          assert.equal(this.getLinkType(true), 'IEEE802_11_RADIO');
          assert.equal(this.getLinkType(), 'IEEE802_11');
          done();
        });

    });

    it('infers the from and to types when piping', function (done) {

      var capture = new dot11.capture.Replay(captures.small.path);
      var extractor = new Extractor();

      capture
        .pipe(extractor)
        .once('readable', function () {
          assert.equal(this.getLinkType(true), 'IEEE802_11_RADIO');
          assert.equal(this.getLinkType(), 'IEEE802_11');
          done();
        });

    });

    it('activates on read', function (done) {

      var capture = new dot11.capture.Replay(captures.small.path);
      var extractor = new Extractor({
        fromLinkType: 'IEEE802_11_RADIO',
        toLinkType: 'IEEE802_11'
      });

      capture
        .on('data', function (buf) { extractor.write(buf); });

      extractor
        .once('readable', function () {
          assert.equal(this.getLinkType(true), 'IEEE802_11_RADIO');
          assert.equal(this.getLinkType(), 'IEEE802_11');
          done();
        });

    });

    it('extracts all valid packets', function (done) {

      var capture = new dot11.capture.Replay(captures.large.path);
      var extractor = new Extractor({toLinkType: 'IEEE802_11'});
      var nPackets = 0;

      capture
        .pipe(extractor)
        .on('data', function () { nPackets++; })
        .on('end', function () {
          assert.equal(nPackets, captures.large.length);
          done();
        });

    });

    it('emits end when the writable side finished', function (done) {

      var capture = new dot11.capture.Replay(captures.large.path);
      var extractor = new Extractor({
        fromLinkType: 'IEEE802_11_RADIO',
        toLinkType: 'IEEE802_11'
      });
      var nPackets = 0;

      capture
        .on('data', function (buf) { extractor.write(buf); })
        .on('end', function () { extractor.end(); });

      extractor
        .on('data', function () { nPackets++; })
        .on('end', function () {
          assert.equal(nPackets, captures.large.length);
          done();
        });

    });

    it('extracts IEEE802_11_RADIO to IEEE802_11', function (done) {

      var capture = new dot11.capture.Replay(captures.small.path);
      var extractor = new Extractor({
        fromLinkType: 'IEEE802_11_RADIO',
        toLinkType: 'IEEE802_11'
      });
      var decoder = new dot11.transform.Decoder();

      capture
        .pipe(extractor)
        .pipe(decoder)
        .once('readable', function () {
          assert.equal(this.getLinkType(), 'IEEE802_11');
          assert.deepEqual(this.read() , {
            version: 0,
            type: 'mgmt',
            subType: 'beacon',
            toDs: 0,
            fromDs: 0,
            retry: 0,
            powerMgmt: 0,
            moreData: 0,
            moreFrag: 0,
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
