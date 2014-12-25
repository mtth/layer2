/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var dot11 = require('../dot11'),
      assert = require('assert'),
      fs = require('fs');

  var smallCapture = {
    path: './test/dat/mesh3.pcap',
    length: 3
  };

  var largeCapture = {
    path: './test/dat/mesh780.pcap',
    length: 780
  };

  describe('Replay capture', function () {

    // Test reading an entire file.

    it('can read an entire file', function (done) {

      var nPackets = 0;

      new dot11.capture.Replay(largeCapture.path)
        .on('data', function () { nPackets++; })
        .on('end', function () {
          assert.equal(nPackets, largeCapture.length);
          done();
        });

    });

    it('can be read by several handlers', function (done) {

      var nPackets1 = 0;
      var nPackets2 = 0;
      var nPackets3 = 0;

      new dot11.capture.Replay(largeCapture.path)
        .on('data', function () { nPackets1++; })
        .on('data', function () { nPackets2++; })
        .on('data', function () { nPackets3++; })
        .on('end', function () {
          assert.equal(nPackets1, largeCapture.length);
          assert.equal(nPackets2, largeCapture.length);
          assert.equal(nPackets3, largeCapture.length);
          done();
        });

    });

    it('closes automatically after reading a file', function (done) {

      function onEnd(capture, cb) { assert.ok(capture.isClosed()); cb(); }

      new dot11.capture.Replay(largeCapture.path)
        .on('data', function () { assert.ok(!this.isClosed()); })
        .on('end', function () { onEnd(this, done); });

    });

    it('supports closing after a few packets', function (done) {

      var nPackets = 0;
      var readPackets = 10;

      new dot11.capture.Replay(largeCapture.path)
        .on('data', function () {
          if (nPackets++ == readPackets) this.close();
        })
        .on('end', function () {
          assert.ok(nPackets < readPackets + 5); // Small margin.
          done();
        });

    });

    it('supports closing after a timeout', function (done) {

      var capture = new dot11.capture.Replay(largeCapture.path, {
        batchSize: 2 // Small enough to guarantee it won't be read in one go.
      });
      var nPackets = 0;

      capture
        .on('data', function () { nPackets++; })
        .on('end', function () {
          assert.ok(nPackets > 0);
          assert.ok(nPackets < largeCapture.length);
          done();
        });
      setTimeout(function () { capture.close(); }, 1);

    });

    // Test direct calls to read.

    function testReadSinglePacket(batchSize, callback) {

      new dot11.capture.Replay(smallCapture.path, {batchSize: batchSize })
        .once('readable', function () {
          assert.ok(this.read() !== null);
          this.close();
          callback();
        });

    }

    it('can read a packet when the batch size is 1', function (done) {

      testReadSinglePacket(1, done);

    });

    it('can read a packet when the batch size is small', function (done) {
      // I.e. batch size smaller than packets in the file.

      testReadSinglePacket(2, done);

    });

    it('can read a packet when the batch size matches', function (done) {
      // I.e. batch size equal to packets in the file.

      testReadSinglePacket(3, done);

    });

    it('can read a packet when the batch size is large', function (done) {
      // I.e. batch size greater than packets in the file.

      testReadSinglePacket(4, done);

    });

    // Test when attaching a data handler.

    function testDispatching(batchSize, callback) {

      var totalPackets = largeCapture.length;
      var totalFetches = Math.ceil(totalPackets / batchSize) + 1;
      var nPackets = 0;
      var nFetches = 0;

      new dot11.capture.Replay(largeCapture.path, {batchSize: batchSize})
        .on('data', function () { nPackets++; })
        .on('fetch', function (ratio) {
          nFetches++;
          if (nFetches < totalFetches - 1) assert.equal(ratio, 1);
          if (nFetches === totalFetches) assert.equal(ratio, 0);
        })
        .on('end', function () {
          assert.equal(nPackets, totalPackets);
          assert.equal(nFetches, totalFetches);
          callback();
        });

    }

    it('dispatches with a batch size of 1', function (done) {

      testDispatching(1, done);

    });

    it('dispatches with a small batch size', function (done) {

      testDispatching(100, done);

    });

    it('dispatches with a matching batch size', function (done) {

      testDispatching(780, done);

    });

    it('dispatches with a large batch size', function (done) {

      testDispatching(1000, done);

    });

    // Test that the packets are correct.

    function bufferEquals(a, b) {

      if (!Buffer.isBuffer(a) || !Buffer.isBuffer(b)) {
        return undefined;
      }
      if (a.length !== b.length) {
        return false;
      }
      for (var i = 0; i < a.length; i++) {
        if (a[i] !== b[i]) {
          return false;
        }
      }
      return true;

    }

    it('yields correct packets', function (done) {

      var index = 0;
      var packets = {
        0: new Buffer('000020006708040054c6b82400000000220cdaa002000000400100003c14241180000000ffffffffffff06037f07a01606037f07a016b0773a40cb260000000064000105000a667265656273642d617001088c129824b048606c030124050400010000072a5553202401112801112c01113001113401173801173c011740011795011e99011e9d011ea1011ea5011e200100dd180050f2020101000003a4000027a4000042435e0062322f00', 'hex'),
        189: new Buffer('00002000670804006d5d2725000000002230d9a001000000400100003c142411d40000000019e3d3535246e97687', 'hex'),
        218: new Buffer('0000200067080400b51e2a2500000000220cd7a001000000400100003c14241108020000ffffffffffff06037f07a0160016cbace5f95083aaaa0300000008004500009be957000040117b3c0a0000c00a0000ff0277027700870d3b663034652033206970703a2f2f31302e302e302e3139323a3633312f7072696e746572732f41646f626550444638202222202241646f62652050444620382e3022202241646f62652050444620333031362e31303222206a6f622d7368656574733d6e6f6e652c6e6f6e65206c656173652d6475726174696f6e3d3330300a', 'hex'),
        491: new Buffer('000020006708040059b76f2500000000220cd8a001000000400100003c14241188020000ffffffffffff00037f07a0160019e3d35352208f00002001011f540500000019e3d35352aaaa03000000080600010800060400010019e3d35352a9fef7000000000000004f673238', 'hex')
      };

      new dot11.capture.Replay(largeCapture.path)
        .on('data', function (data) {
          var original = packets[index++];
          if (original) {
            assert.ok(bufferEquals(data, original));
          }
        })
        .on('end', function () { done(); });

    });

    it('returns the correct snapshot length', function () {

        var capture = new dot11.capture.Replay(largeCapture.path);
        assert.equal(capture.getSnapLen(), 65535);

    });

  });

  describe('Save capture', function () {

    function checkEqual(pathA, pathB) {

      var replayA = new dot11.capture.Replay(pathA);
      var replayB = new dot11.capture.Replay(pathB);

      var a, b;
      while ((a = replayA.read()) !== null || (b = replayB.read()) !== null) {
        assert.deepEqual(a, b);
      }

    }

    it('throws an error when using an empty or invalid datalink', function () {

      var savePath = './test/empty.pcap';
      assert.throws(function () { new dot11.capture.Save(savePath); });
      assert.throws(function () { new dot11.capture.Save(savePath, 'FOO'); });

    });

    it('can be written to', function (done) {

      var savePath = './test/write.pcap';
      var replay = new dot11.capture.Replay(smallCapture.path);
      var save = new dot11.capture.Save(savePath, replay.getDatalink());

      replay
        .on('data', function (buf) { save.write(buf); })
        .on('end', function () {
          save.end();
          checkEqual(savePath, smallCapture.path);
          done();
        });

      after(function () { fs.unlink(savePath); });

    });

    it('can be piped to', function (done) {

      var savePath = './test/pipe.pcap';
      var replay = new dot11.capture.Replay(smallCapture.path);
      var save = new dot11.capture.Save(savePath, replay.getDatalink());

      replay
        .pipe(save)
        .on('finish', function () {
          checkEqual(savePath, smallCapture.path);
          done();
        });

      after(function () { fs.unlink(savePath); });

    });

  });

})();
