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

    });

    describe('Pcap Wrapper', function () {

      var maybeIt = maybe(it, hasActiveDevice());

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

      it('fetches frames async', function (done) {

        var isAsync = false;
        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');
        var buf = new Buffer(3 * wrapper.getMaxFrameSize());

        wrapper
          .fetch(3, buf, function (err, headers, broke) {
            assert.ok(err === null);
            assert.equal(headers.length, 3);
            assert.deepEqual(
              buf.slice(0, headers[0].capLen),
              new Buffer('000020006708040054c6b82400000000220cdaa002000000400100003c14241180000000ffffffffffff06037f07a01606037f07a016b0773a40cb260000000064000105000a667265656273642d617001088c129824b048606c030124050400010000072a5553202401112801112c01113001113401173801173c011740011795011e99011e9d011ea1011ea5011e200100dd180050f2020101000003a4000027a4000042435e0062322f00', 'hex')
            );
            assert.ok(!broke);
            wrapper.close();
            done();
          });

        setImmediate(function () { isAsync = true; });

      });

      it('fetches frames async and breaks', function (done) {

        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');

        var buf = new Buffer(wrapper.getMaxFrameSize());
        // Break after first frame.

        wrapper
          .fetch(3, buf, function (err, headers, broke) {
            assert.ok(err === null);
            assert.equal(headers.length, 1);
            assert.ok(broke);
            wrapper.close();
            done();
          });

      });

      it('throws an error when fetching concurrently', function (done) {

        var ran = false;
        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');

        var buf = new Buffer(wrapper.getMaxFrameSize());

        wrapper.fetch(1, buf, function () {
          wrapper.close();
          assert.ok(ran);
          done();
        });

        assert.throws(function () {
          ran = true;
          wrapper.fetch(1, buf, function () {});
        });

      });

      it('throws an error when fetching after close', function (done) {

        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');
        var buf = new Buffer(wrapper.getMaxFrameSize());

        wrapper.close();
        assert.throws(function () {
          wrapper.fetch(1, buf, function () {});
        });
        done();

      });

      it('fetches after a break', function (done) {

        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');

        var buf = new Buffer(wrapper.getMaxFrameSize());

        wrapper
          .fetch(3, buf, function (err, headers, broke) {
            assert.ok(err === null);
            assert.equal(headers.length, 1);
            assert.ok(broke);
            wrapper.fetch(1, buf, function (err, headers, broke) {
              assert.ok(err === null);
              assert.equal(headers.length, 1);
              assert.ok(broke);
              wrapper.close();
              done();
            });
          });

      });

      it('fetches all frames from a save file', function (done) {

        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');

        var buf = new Buffer(10 * wrapper.getMaxFrameSize());

        wrapper
          .fetch(-1, buf, function (err, headers, broke) {
            assert.ok(err === null);
            assert.equal(headers.length, 780);
            assert.ok(!broke);
            wrapper.close();
            done();
          });

      });

      it('fetches no frames after finishing a save file', function (done) {

        var wrapper = new addon.PcapWrapper()
          .fromSavefile('./test/dat/mesh780.pcap');

        var buf = new Buffer(10 * wrapper.getMaxFrameSize());

        wrapper
          .fetch(1000, buf, function (err, headers, broke) {
            assert.ok(err === null);
            assert.equal(headers.length, 780);
            assert.ok(!broke);
            wrapper.fetch(1, buf, function (err, headers) {
              assert.ok(err === null);
              assert.equal(headers.length, 0);
              wrapper.close();
              done();
            });
          });

      });

      maybeIt('fails if activated with no buffer size', function () {

        assert.throws(function () {
          new addon.PcapWrapper()
            .fromDevice(addon.getDefaultDevice())
            .activate();
        });

      });

      maybeIt('fails if the buffer is too small', function () {

        var wrapper = new addon.PcapWrapper()
          .fromDevice(addon.getDefaultDevice())
          .setBufferSize(100)
          .activate();

        assert.throws(function () {
          wrapper.dispatch(3, new Buffer(50), function () {});
        });

        assert.throws(function () {
          wrapper.fetch(1, new Buffer(50), function () {});
        });

      });

    });

    function hasActiveDevice() {
      // Check whether there is a semi active network we can listen to.

      var device;
      try {
        device = addon.getDefaultDevice();
      } catch (err) {
        device = null;
      }
      return !!device;

    }

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

  function maybe(fn, predicate) {
    // Skip test if predicate is false (fn should be `describe` or `it`).

    return predicate ? fn : fn.skip;

  }

})();
