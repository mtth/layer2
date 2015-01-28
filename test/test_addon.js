/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var assert = require('assert'),
      addon = require('../lib/utils').requireAddon();

  describe('Addon', function () {

    describe('Dispatcher', function () {

      var maybeIt = maybe(it, hasActiveDevice()); // jshint ignore: line

      it('can be instantiated on a file', function () {

        var wrapper = addon.Dispatcher
          .fromSavefile('./test/dat/mesh.pcap');

        assert.equal(wrapper.getSnaplen(), 65535);
        assert.equal(wrapper.getDatalink(), 127); // Radiotap.

      });

      it('throws an error when instantiated on a missing file', function () {

        assert.throws(function () {
          addon.Dispatcher.fromSavefile('./foobar');
        });

      });

      it('throws an error when no handle is present', function () {
        // Most importantly, it doesn't segfault.

        assert.throws(function () {
          addon.Dispatcher.getSnaplen();
        });

      });

      it('fetches frames async', function (done) {

        var isAsync = false;
        var wrapper = addon.Dispatcher
          .fromSavefile('./test/dat/mesh.pcap');
        var buf = new Buffer(3 * wrapper.getSnaplen());
        var nFrames = 0;

        wrapper
          .fetch(3, buf, function (err, start, end) {
            assert.ok(err === null);

            if (end === 0) { // Flag.
              assert.equal(start, 3);
              return;
            }

            if (++nFrames === 3) {
              // assert.deepEqual(
              //   buf.slice(0, headers[0].capLen),
              //   new Buffer('000020006708040054c6b82400000000220cdaa002000000400100003c14241180000000ffffffffffff06037f07a01606037f07a016b0773a40cb260000000064000105000a667265656273642d617001088c129824b048606c030124050400010000072a5553202401112801112c01113001113401173801173c011740011795011e99011e9d011ea1011ea5011e200100dd180050f2020101000003a4000027a4000042435e0062322f00', 'hex')
              // );
              wrapper.close();
              done();
            }

          });

        setImmediate(function () { isAsync = true; });

      });

      it('fetches frames async and breaks', function (done) {

        var wrapper = new addon.Dispatcher
          .fromSavefile('./test/dat/mesh.pcap');

        var buf = new Buffer(1);
        var nFrames = 0;
        // Break after first frame.

        wrapper
          .fetch(3, buf, function (err, start, end) {

            if (end === 0) {
              assert.ok(err === null);
              assert.equal(start, 1);
              return;
            }

            if (++nFrames === 1) {
              wrapper.close();
              done();
            }

          });

      });

      it('throws an error when fetching concurrently', function (done) {

        var ran = false;
        var wrapper = new addon.Dispatcher
          .fromSavefile('./test/dat/mesh.pcap');

        var buf = new Buffer(1e6);
        var nFrames = 0;

        wrapper.fetch(-1, buf, function () {
          if (++nFrames === 780) {
            wrapper.close();
            assert.ok(ran);
            done();
          }
        });

        assert.throws(function () {
          ran = true;
          wrapper.fetch(1, buf, function () {});
        });

      });

      it('throws an error when fetching after close', function (done) {

        var wrapper = new addon.Dispatcher
          .fromSavefile('./test/dat/mesh.pcap');
        var buf = new Buffer(wrapper.getSnaplen());

        wrapper.close();
        assert.throws(function () {
          wrapper.fetch(1, buf, function () {});
        });
        done();

      });

      it('fetches all frames from a save file', function (done) {

        var wrapper = new addon.Dispatcher
          .fromSavefile('./test/dat/mesh.pcap');

        var buf = new Buffer(1e6);
        var nFrames = 0;

        wrapper
          .fetch(-1, buf, function (err, start, end) {
            if (end === 0) {
              assert.equal(start, 780);
              return;
            }
            if (++nFrames === 780) {
              wrapper.close();
              done();
            }
          });

      });

      it('fetches no frames after finishing a save file', function (done) {

        var wrapper = new addon.Dispatcher
          .fromSavefile('./test/dat/mesh.pcap');

        var buf = new Buffer(1e6);
        var nFrames = 0;

        wrapper
          .fetch(-1, buf, function (err, start, end) {
            if (end === 0) {
              assert.equal(start, 780);
              return;
            }
            if (++nFrames === 780) {
              wrapper.fetch(1, buf, function (err, start, end) {
                assert.equal(start, 0);
                assert.equal(end, 0);
                wrapper.close();
                done();
              });
            }
          });

      });

      // Tests that require a live interface.

      // TODO: maybeIt('', function () {});

    });

    describe('Frame', function () {

      it('supports radiotap', function () {

        var buf = new Buffer('000020006708040054c6b82400000000220cdaa002000000400100003c142411b4007c013ce072e6612bcc03fadc202a719fe3d6', 'hex');
        var frame = new addon.Frame(127, buf); // radiotap link type
        var pdu = frame.getPdu(3); // radiotap pdutype
        assert.deepEqual(pdu.getChannel(), {freq: 5180, type: 320});
        assert.equal(pdu.getRate(), 12);
        assert.equal(pdu.getSize(), 48);
        assert.deepEqual(pdu.getDbm(), {signal: 218, noise: 160});

      });

    });

    describe('Utilities', function () {

      it('can parse MAC addresses', function () {

        var buf = new Buffer('0123456789ab001122334455', 'hex');
        assert.equal(addon.readMacAddr(buf, 0), '01:23:45:67:89:ab');
        assert.equal(addon.readMacAddr(buf, 6), '00:11:22:33:44:55');
        assert.throws(function () { addon.readMacAddr(buf, 8); });

      });

      it('can return link infos', function () {

        assert.equal(addon.getLinkInfo(-5), null); // Invalid type.

        assert.deepEqual(
          addon.getLinkInfo(127),
          {
            name: 'IEEE802_11_RADIO',
            description: '802.11 plus radiotap header'
          }
        );

        assert.deepEqual(
          addon.getLinkInfo(0),
          {
            name: 'NULL',
            description: 'BSD loopback'
          }
        );

      });

    });

    function hasActiveDevice() {
      // Check whether there is an active network we can listen to.

      var device;
      try {
        device = addon.getDefaultDevice();
      } catch (err) {
        device = null;
      }
      return !!device;

    }

  });

  function maybe(fn, predicate) {
    // Skip test if predicate is false (fn should be `describe` or `it`).

    return predicate ? fn : fn.skip;

  }

})();
