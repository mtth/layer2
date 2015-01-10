/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var assert = require('assert'),
      fs = require('fs'),
      path = require('path');

  describe('Addon', function () {

    var addon = requireAddon();

    describe('Utilities', function () {

      it('can parse MAC addresses', function () {

        var buf = new Buffer('0123456789ab001122334455', 'hex');
        assert.equal(addon.readMacAddr(buf, 0), '01:23:45:67:89:ab');
        assert.equal(addon.readMacAddr(buf, 6), '00:11:22:33:44:55');
        assert.throws(function () { addon.readMacAddr(buf, 8); });

      });

      it('can pretend decode', function () {

        var buf = new Buffer('000020006708040054c6b82400000000220cdaa002000000400100003c142411b4007c013ce072e6612bcc03fadc202a719fe3d6', 'hex');
        // assert.equal(addon.decoders.IEEE802_11_RADIO(buf, 0));
        buf = new Buffer('000019006f0800009627433d00000000560c1c164001d0a4010862', 'hex');
        // assert.equal(addon.decoders.IEEE802_11_RADIO(buf, 0));

      });

    });

    describe('Pcap Wrapper', function () {

      it('can be instantiated on a file', function () {

        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');

        assert.equal(wrapper.getMaxFrameSize(), 65535);
        assert.equal(wrapper.getLinkType(), 'IEEE802_11_RADIO');
        wrapper.close();

      });

      it('throws an error when instantiated on a missing file', function () {

        assert.throws(function () {
          new addon.PcapWrapper().fromSavefile('./foobar');
        });

      });

      it('throws an error when no handle is present', function () {
        // Most importantly, it doesn't segfault.

        assert.throws(function () {
          new addon.PcapWrapper().getMaxFrameSize();
        });

      });

      it('dispatches frames async', function (done) {

        var isAsync = false;
        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');
        var buf = new Buffer(3 * wrapper.getMaxFrameSize());

        wrapper
          .dispatch(3, buf, function (err, headers, broke) {
            assert.ok(err === null);
            assert.equal(headers.length, 3);
            assert.ok(!broke);
            wrapper.close();
            done();
          });

        setTimeout(function () { isAsync = true; }, 0);

      });

      it('dispatches frames async and breaks', function (done) {

        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');

        var buf = new Buffer(wrapper.getMaxFrameSize());
        // Break after first frame.

        wrapper
          .dispatch(3, buf, function (err, headers, broke) {
            assert.ok(err === null);
            assert.equal(headers.length, 1);
            assert.ok(broke);
            wrapper.close();
            done();
          });

      });

      it('throws an error when dispatching concurrently', function (done) {

        var ran = false;
        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');

        var buf = new Buffer(wrapper.getMaxFrameSize());

        wrapper.dispatch(1, buf, function () {
          wrapper.close();
          assert.ok(ran);
          done();
        });

        assert.throws(function () {
          ran = true;
          wrapper.dispatch(1, buf, function () {});
        });

      });

      it('dispatches after a break', function (done) {

        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');

        var buf = new Buffer(wrapper.getMaxFrameSize());

        wrapper
          .dispatch(3, buf, function (err, headers, broke) {
            assert.ok(err === null);
            assert.equal(headers.length, 1);
            assert.ok(broke);
            wrapper.dispatch(1, buf, function (err, headers, broke) {
              assert.ok(err === null);
              assert.equal(headers.length, 1);
              assert.ok(broke);
              done();
            });
          });

      });

      it('dispatches all frames from a save file', function (done) {

        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');

        var buf = new Buffer(10 * wrapper.getMaxFrameSize());

        wrapper
          .dispatch(1000, buf, function (err, headers, broke) {
            assert.ok(err === null);
            assert.equal(headers.length, 780);
            assert.ok(!broke);
            done();
          });

      });

      it('dispatches no frames after finishing a save file', function (done) {

        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');

        var buf = new Buffer(10 * wrapper.getMaxFrameSize());

        wrapper
          .dispatch(1000, buf, function (err, headers, broke) {
            assert.ok(err === null);
            assert.equal(headers.length, 780);
            assert.ok(!broke);
            wrapper.dispatch(1, buf, function (err, headers) {
              assert.ok(err === null);
              assert.equal(headers.length, 0);
              done();
            });
          });

      });

    });

  });

  function requireAddon() {
    // Variant of utils where either Debug or Release can be used (using Debug
    // if available).

    var buildFolder = path.join(__dirname, '..', 'build');
    return require(
      fs.existsSync(path.join(buildFolder, 'Debug')) ?
      path.join(buildFolder, 'Debug') :
      path.join(buildFolder, 'Release')
    );

  }

})();
