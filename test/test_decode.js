/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var assert = require('assert'),
      layer2 = require('../lib');

  var capturePath = './test/dat/mixed.pcap';

  describe('Decoder', function () {

    var Decoder = layer2.Decoder;

    describe('stream', function () {

      it('can be piped to and read from', function (done) {

        var capture = new layer2.capture.Replay(capturePath);
        var decoder = new Decoder({linkType: capture.getLinkType()});
        var readFrame = false;

        capture
          .on('close', function () {
            assert.ok(readFrame);
            done();
          })
          .pipe(decoder)
          .on('readable', function () {
            assert.equal(this.getLinkType(), 105);
            var frame = this.read();
            if (!readFrame) {
              assert.ok(frame && frame instanceof layer2.Frame);
              readFrame = true;
            }
          });

      });

      it('can be piped to and listened to', function (done) {

        var capture = new layer2.capture.Replay(capturePath);
        var decoder = new Decoder({linkType: capture.getLinkType()});
        var nFrames = 0;

        capture
          .on('close', function () {
            assert.equal(nFrames, 134);
            done();
          })
          .pipe(decoder)
          .on('data', function () { nFrames++; });

      });

      it('can infer the link type', function (done) {

        var capture = new layer2.capture.Replay(capturePath);
        var decoder = new Decoder();
        var linkType;

        capture
          .on('close', function () {
            assert.equal(linkType, 105);
            done();
          })
          .pipe(decoder)
          .once('readable', function () {
            linkType = this.getLinkType();
          })
          .on('data', function () {});

      });

      it('emits end when the writable side finished', function (done) {

        var capture = new layer2.capture.Replay(capturePath);
        var decoder = new Decoder({linkType: capture.getLinkType()});
        var nFrames = 0;

        capture
          .on('data', function (buf) { decoder.write(buf); })
          .on('end', function () { decoder.end(); });

        decoder
          .on('data', function (data) {
            assert.ok(data && typeof data == 'object');
            nFrames++;
          })
          .on('end', function () {
            assert.equal(nFrames, 134);
            done();
          });

      });

      it('emits events when a frame fails to decode', function (done) {

        var capture = new layer2.capture.Replay(capturePath);
        var decoder = new Decoder();
        var nFrames = 0;

        capture
          .pipe(decoder)
          .on('data', function () {})
          .on('invalid', function (frame) {
            assert.ok(frame !== null);
            nFrames++;
          })
          .on('end', function () {
            assert.equal(nFrames, 32);
            done();
          });

      });

    });

  });

})();
