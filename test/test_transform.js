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
    mixed: { // With invalid frames.
      path: './test/dat/mixed.pcap',
      length: {valid: 48, invalid: 118}
    }
  };

  describe('Transform', function () {

    describe('Decoder', function () {

      var Decoder = dot11.transform.Decoder;

      it('can be piped to and read from', function (done) {

        var capture = new dot11.capture.Replay(captures.small.path);
        var decoder = new Decoder({linkType: capture.getLinkType()});

        capture
          .pipe(decoder)
          .once('readable', function () {
            assert.equal(this.getLinkType(), 'IEEE802_11_RADIO');
            var frame = this.read();
            assert.deepEqual(frame, {
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
        var nFrames = 0;

        capture
          .pipe(decoder)
          .on('data', function (data) {
            assert.ok(data && typeof data == 'object');
            nFrames++;
          })
          .on('end', function () {
            assert.equal(this.getLinkType(), 'IEEE802_11_RADIO');
            assert.equal(nFrames, captures.small.length);
            done();
          });

      });

      it('emits end when the writable side finished', function (done) {

        var capture = new dot11.capture.Replay(captures.large.path);
        var decoder = new Decoder({linkType: capture.getLinkType()});
        var nFrames = 0;

        capture
          .on('data', function (buf) { decoder.write(buf); })
          .on('end', function () { decoder.end(); });

        decoder
          .on('data', function () { nFrames++; })
          .on('end', function () {
            assert.equal(nFrames, captures.large.length);
            done();
          });

      });

      it('emits events when a frame fails to decode', function (done) {

        var capture = new dot11.capture.Replay(captures.mixed.path);
        var decoder = new Decoder();
        var nValidFrames = 0;
        var nInvalidFrames = 0;

        capture
          .pipe(decoder)
          .on('data', function (data) {
            assert.ok(data && typeof data == 'object');
            nValidFrames++;
          })
          .on('invalid', function (err) {
            assert.ok(err.data && typeof err.data == 'object');
            nInvalidFrames++;
          })
          .on('end', function () {
            assert.equal(nValidFrames, captures.mixed.length.valid);
            assert.equal(nInvalidFrames, captures.mixed.length.invalid);
            done();
          });

      });

    });

    describe('Extractor', function () {

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

      it('extracts all valid frames', function (done) {

        var capture = new dot11.capture.Replay(captures.large.path);
        var extractor = new Extractor({toLinkType: 'IEEE802_11'});
        var nFrames = 0;

        capture
          .pipe(extractor)
          .on('data', function () { nFrames++; })
          .on('end', function () {
            assert.equal(nFrames, captures.large.length);
            done();
          });

      });

      it('emits end when the writable side finished', function (done) {

        var capture = new dot11.capture.Replay(captures.large.path);
        var extractor = new Extractor({
          fromLinkType: 'IEEE802_11_RADIO',
          toLinkType: 'IEEE802_11'
        });
        var nFrames = 0;

        capture
          .on('data', function (buf) { extractor.write(buf); })
          .on('end', function () { extractor.end(); });

        extractor
          .on('data', function () { nFrames++; })
          .on('end', function () {
            assert.equal(nFrames, captures.large.length);
            done();
          });

      });

      it('extracts IEEE802_11_RADIO to IEEE802_11', function (done) {

        var capture = new dot11.capture.Replay(captures.small.path);
        var extractor = new Extractor({
          fromLinkType: 'IEEE802_11_RADIO',
          toLinkType: 'IEEE802_11',
        });
        var decoder = new dot11.transform.Decoder({assumeValid: true});

        capture
          .pipe(extractor)
          .pipe(decoder)
          .once('readable', function () {
            assert.equal(this.getLinkType(), 'IEEE802_11');
            done();
          });

      });

    });

  });

})();
