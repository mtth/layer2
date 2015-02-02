/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var assert = require('assert'),
      addon = require('../lib/utils').requireAddon(),
      path = require('path'),
      fs = require('fs');

  describe('Addon', function () {

    describe('Frame', function () {

      var Frame = addon.Frame;
      var buf = new Buffer('000020006708040054c6b82400000000220cdaa002000000400100003c14241180000000ffffffffffff06037f07a01606037f07a016b0773a40cb260000000064000105000a667265656273642d617001088c129824b048606c030124050400010000072a5553202401112801112c01113001113401173801173c011740011795011e99011e9d011ea1011ea5011e200100dd180050f2020101000003a4000027a4000042435e0062322f00', 'hex'); // Valid Radiotap buffer.

      it('can be instantiated from a valid buffer', function () {

        var frame = new Frame(127, buf); // radiotap link type
        assert.ok(frame.isValid());
        assert.equal(frame.size(), buf.length);

      });

      it('creates fake headers from the buffer', function () {

        var frame = new Frame(127, buf); // radiotap link type
        var header = frame.getHeader();
        assert.equal(header.length, buf.length);
        assert.equal(header.capturedLength, buf.length);
        assert.ok(new Date().getTime() - header.time < 10);

      });

      it('can be instantiated from a invalid buffer', function () {

        var buf = new Buffer('0020006708040054c6b82400000000220cdaa002000000400100003c142411b4007c013ce072e6612bcc03fadc202a719fe3d7', 'hex');
        var frame = new Frame(127, buf);
        assert.ok(!frame.isValid());

      });

      it('returns its data in a buffer', function () {

        var frame = new Frame(127, buf); // radiotap link type
        assert.deepEqual(buf, frame.getData());

      });

      it('returns an existing PDU', function () {

        var frame = new Frame(127, buf);
        var pdu = frame.getPdu(3); // radiotap pdutype
        assert.ok(pdu !== null);
        assert.deepEqual(pdu.getChannel(), {freq: 5180, type: 320});
        assert.equal(pdu.getRate(), 12);
        assert.equal(pdu.getSize(), 172);
        assert.deepEqual(pdu.getDbm(), {signal: 218, noise: 160});

      });

      it('returns null on missing PDU', function () {

        var frame = new Frame(127, buf);
        var pdu = frame.getPdu(2); // ethernet pdutype
        assert.ok(pdu === null);

      });

    });

    describe('Dispatcher', function () {

      var Dispatcher = addon.Dispatcher;

      var maybeIt = maybe(it, hasActiveDevice()); // jshint ignore: line

      it('can be instantiated on a file', function () {

        var dispatcher = Dispatcher.fromSavefile('./test/dat/mesh.pcap');

        assert.equal(dispatcher.getSnaplen(), 65535);
        assert.equal(dispatcher.getDatalink(), 127); // Radiotap.

      });

      it('throws an error when instantiated on a missing file', function () {

        assert.throws(function () {
          Dispatcher.fromSavefile('./foobar');
        });

      });

      it('throws an error when no handle is present', function () {
        // Most importantly, it doesn't segfault.

        assert.throws(function () {
          Dispatcher.getSnaplen();
        });

      });

      it('dispatches frames async', function (done) {

        var isAsync = false;
        var dispatcher = Dispatcher.fromSavefile('./test/dat/mesh.pcap');

        dispatcher
          .dispatch(3, function (err, iter) {
            assert.ok(err === null);
            var nFrames = 0;
            while (iter.next()) {
              nFrames++;
            }
            assert.ok(isAsync);
            assert.equal(nFrames, 3);
            done();
          });

        setImmediate(function () { isAsync = true; });

      });

      it('throws an error when dispatching concurrently', function (done) {

        var dispatcher = new Dispatcher.fromSavefile('./test/dat/mesh.pcap');

        dispatcher
          .dispatch(-1, function () {})
          .dispatch(-1, function (err) {
            assert.ok(err !== null);
            done();
          });

      });

      it('dispatches all frames from a save file', function (done) {

        var dispatcher = new Dispatcher
          .fromSavefile('./test/dat/mesh.pcap');
        var nFrames = 0;

        dispatcher
          .dispatch(-1, function (err, iter) {
            assert.ok(err === null);
            while (iter.next()) {
              nFrames++;
            }
            assert.equal(nFrames, 780);
            done();
          });

      });

      it('dispatches no frames after finishing a save file', function (done) {

        var dispatcher = new Dispatcher
          .fromSavefile('./test/dat/mesh.pcap');

        dispatcher
          .dispatch(-1, function (err) {
            assert.ok(err === null);
            dispatcher.dispatch(1, function (err, iter) {
              assert.ok(err === null);
              assert.ok(iter.next() === null);
              done();
            });
          });

      });

      it('returns frames with capture headers', function (done) {

        Dispatcher.fromSavefile('./test/dat/mesh.pcap')
          .dispatch(1, function (err, iter) {
            var frame = iter.next();
            assert.deepEqual(
              frame.getHeader(),
              {
                time: 1247544845137.966,
                length: 172,
                capturedLength: 172
              }
            );
            done();
          });

      });

      it('can be activated from a dead interface', function () {

        var dispatcher = Dispatcher.fromDead(127, 200);
        assert.equal(dispatcher.getDatalink(), 127);
        assert.equal(dispatcher.getSnaplen(), 200);

      });

      it('sets the properties on a savefile', function () {

        var fpath = fromName('set_properties.pcap');

        Dispatcher
          .fromDead(127, 500)
          .setSavefile(fpath)
          .setSavefile(null); // Flush.

        var dispatcher = Dispatcher.fromSavefile(fpath);
        assert.equal(dispatcher.getDatalink(), 127);
        assert.equal(dispatcher.getSnaplen(), 500);

      });

      it('can write a frame to a file', function (done) {

        var fpath = fromName('write_frame.pcap');
        var buf = new Buffer('aabb', 'hex');

        Dispatcher.fromDead(127, 65535)
          .setSavefile(fpath)
          .dump(new addon.Frame(127, buf))
          .setSavefile(null); // Flush.

        Dispatcher.fromSavefile(fpath)
          .dispatch(1, function (err, iter) {
            var frame = iter.next();
            assert.ok(frame !== null);
            assert.deepEqual(frame.getData(), buf);
            done();
          });

      });

      it('fails when writing without setting the file', function () {

        assert.throws(function () {
          Dispatcher.fromDead(127, 65535)
            .dump(new addon.Frame(127, new Buffer(0)));
        });

      });

      it('fails when writing something else than a frame', function () {

        assert.throws(function () {
          Dispatcher.fromDead(127, 65535)
            .setSavefile(fromName('write_non_frame.pcap'))
            .dump(new Buffer(0));
        });

      });

      // TODO: Tests that require a live interface.

      // Helpers.

      // Name and delete file after test.
      function fromName(fname) {

        var fpath = path.join(__dirname, 'dat', fname);
        after(function () { fs.unlink(fpath); });
        return fpath;

      }

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
