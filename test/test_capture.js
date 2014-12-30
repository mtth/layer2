/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var dot11 = require('../lib'),
      assert = require('assert'),
      crypto = require('crypto'),
      fs = require('fs'),
      path = require('path'),
      http = require('http');

  // Sample captures used in most test cases.

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

  describe('Capture', function () {

    // Replays.

    describe('Replay', function () {

      var Replay = dot11.capture.Replay;

      it('returns the correct maximum packet size', function () {

        var capture = new Replay(captures.large.path);
        assert.equal(capture.getMaxPacketSize(), 65535);

      });

      it('returns the correct link type', function () {

        var capture = new Replay(captures.large.path);
        assert.equal(capture.getLinkType(), 'IEEE802_11_RADIO');

      });

      it('returns the correct path', function () {

        var capture = new Replay(captures.large.path);
        assert.equal(capture.getPath(), captures.large.path);

      });

      it('can read an entire file', function (done) {

        var nPackets = 0;

        new Replay(captures.large.path)
          .on('data', function () { nPackets++; })
          .on('end', function () {
            assert.equal(nPackets, captures.large.length);
            done();
          });

      });

      it('can read an entire file and breaks if necessary', function (done) {

        var nPackets = 0;
        var nBreaks = 0;

        new Replay(captures.large.path, {
          bufferSize: 100000 // Not big enough for an entire batch.
        })
          .on('break', function () { nBreaks++; })
          .on('data', function () { nPackets++; })
          .on('end', function () {
            assert.equal(nPackets, captures.large.length);
            assert.ok(nBreaks > 0);
            done();
          });

      });

      it('breaks each time if the buffer size is small', function (done) {
        // I.e. equal to the max packet size.

        var nPackets = 0;
        var nBreaks = 0;

        new Replay(captures.large.path, {
          bufferSize: 65535, // Not big enough for an entire batch.
          maxPacketSize: 65535
        })
          .on('break', function () { nBreaks++; })
          .on('data', function () { nPackets++; })
          .on('end', function () {
            assert.equal(nPackets, captures.large.length);
            assert.equal(nBreaks, nPackets);
            done();
          });

      });

      it('can be read by several handlers', function (done) {

        var nPackets1 = 0;
        var nPackets2 = 0;
        var nPackets3 = 0;

        new Replay(captures.large.path)
          .on('data', function () { nPackets1++; })
          .on('data', function () { nPackets2++; })
          .on('data', function () { nPackets3++; })
          .on('end', function () {
            assert.equal(nPackets1, captures.large.length);
            assert.equal(nPackets2, captures.large.length);
            assert.equal(nPackets3, captures.large.length);
            done();
          });

      });

      it('can be read packet by packet', function (done) {

        var packets = [];

        new Replay(captures.small.path)
          .on('readable', function () {
            var buf;
            while ((buf = this.read()) !== null) {
              packets.push(buf);
            }
          })
          .on('end', function () {
            assert.equal(packets.length, captures.small.length);
            done();
          });

      });

      it('closes automatically after reading a file', function (done) {

        var isClosed = false;

        new Replay(captures.large.path)
          .on('data', function () { assert.ok(!isClosed); })
          .on('close', function () { isClosed = true; })
          .on('end', function () {
            setImmediate(function () {
              assert.ok(isClosed); done();
            });
          });

      });

      it('supports closing after a few packets', function (done) {

        var nPackets = 0;

        new Replay(captures.large.path, {batchSize: 1})
          .on('data', function () {
            if (nPackets++ === 10) this.close();
          })
          .on('end', function () {
            assert.ok(nPackets < captures.large.length); // Small margin.
            done();
          });

      });

      it('supports closing after a timeout', function (done) {

        var capture = new Replay(captures.large.path, {
          batchSize: 1 // Small enough to guarantee it won't be read in one go.
        });
        var nPackets = 0;

        capture
          .close(1)
          .on('data', function () { nPackets++; })
          .on('end', function () {
            assert.ok(nPackets > 0);
            assert.ok(nPackets < captures.large.length);
            done();
          });

      });

      it('supports closing after a built-in timeout', function (done) {
        // I.e. lets the event loop run after a while.

        var capture = new Replay(captures.large.path, {
          batchSize: 2 // Small enough to guarantee it won't be read in one go.
        });
        var nPackets = 0;

        capture
          .on('data', function () { nPackets++; })
          .on('end', function () {
            assert.ok(nPackets > 0);
            assert.ok(nPackets < captures.large.length);
            done();
          });
        setTimeout(function () { capture.close(); }, 1);

      });

      it('closes after ending', function (done) {

        var ended = false;

        new Replay(captures.large.path)
          .on('data', function () {})
          .on('end', function () { ended = true; })
          .on('close', function () { done(); });

      });

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

      it('yields correct packets', function (done) {

        var index = 0;
        var packets = {
          0: new Buffer('000020006708040054c6b82400000000220cdaa002000000400100003c14241180000000ffffffffffff06037f07a01606037f07a016b0773a40cb260000000064000105000a667265656273642d617001088c129824b048606c030124050400010000072a5553202401112801112c01113001113401173801173c011740011795011e99011e9d011ea1011ea5011e200100dd180050f2020101000003a4000027a4000042435e0062322f00', 'hex'),
          189: new Buffer('00002000670804006d5d2725000000002230d9a001000000400100003c142411d40000000019e3d3535246e97687', 'hex'),
          218: new Buffer('0000200067080400b51e2a2500000000220cd7a001000000400100003c14241108020000ffffffffffff06037f07a0160016cbace5f95083aaaa0300000008004500009be957000040117b3c0a0000c00a0000ff0277027700870d3b663034652033206970703a2f2f31302e302e302e3139323a3633312f7072696e746572732f41646f626550444638202222202241646f62652050444620382e3022202241646f62652050444620333031362e31303222206a6f622d7368656574733d6e6f6e652c6e6f6e65206c656173652d6475726174696f6e3d3330300a', 'hex'),
          491: new Buffer('000020006708040059b76f2500000000220cd8a001000000400100003c14241188020000ffffffffffff00037f07a0160019e3d35352208f00002001011f540500000019e3d35352aaaa03000000080600010800060400010019e3d35352a9fef7000000000000004f673238', 'hex')
        };

        new dot11.capture.Replay(captures.large.path)
          .on('data', function (data) {
            var original = packets[index++];
            if (original) {
              assert.ok(bufferEquals(data, original));
            }
          })
          .on('end', function () { done(); });

      });

      function testDispatching(batchSize, callback) {

        var totalPackets = captures.large.length;
        var totalFetches = Math.ceil(totalPackets / batchSize) + 1;
        var nPackets = 0;
        var nFetches = 0;

        new Replay(captures.large.path, {batchSize: batchSize})
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

      function testReadSinglePacket(batchSize, callback) {

        new Replay(captures.small.path, {batchSize: batchSize })
          .once('readable', function () {
            assert.ok(this.read() !== null);
            this.close();
            callback();
          });

      }

    });

    // Saves.

    describe('Save', function () {

      var Save = dot11.capture.Save;

      it('returns the correct maximum packet size', function () {

        var savePath = fromName('max_packet_size_default.pcap');
        var save = new Save(savePath, {maxPacketSize: 1024});
        assert.equal(save.getMaxPacketSize(), 1024);

      });

      it('returns the correct link type', function () {

        var savePath = fromName('link_type.pcap');
        var save = new Save(savePath, {linkType: 'IEEE802_11_RADIO'});
        assert.equal(save.getLinkType(), 'IEEE802_11_RADIO');

      });

      it('throws an error when using an empty/invalid link type', function () {

        var savePath = fromName('empty.pcap');

        assert.throws(function () {
          new Save(savePath).write('');
        });
        assert.throws(function () {
          new Save(savePath, {linkType: 'FOO'}).write('');
        });

      });

      it('returns the correct path', function () {

        var savePath = fromName('path.pcap');
        var save = new Save(savePath);
        assert.equal(save.getPath(), savePath);

      });


      it('can be written to', function (done) {

        var savePath = fromName('write.pcap');
        var replay = new dot11.capture.Replay(captures.small.path);
        var save = new Save(savePath, {
          linkType: replay.getLinkType()
        });

        replay
          .on('data', function (buf) { save.write(buf); })
          .on('end', function () { save.end(); }); // Not piping so manual close.

        save
          .on('close', function () {
            checkEqual(this.getPath(), captures.small.path);
            done();
          });


      });

      it('can be piped to', function (done) {

        var savePath = fromName('pipe.pcap');
        var replay = new dot11.capture.Replay(captures.small.path);
        var save = new Save(savePath, {linkType: replay.getLinkType()});

        replay
          .pipe(save)
          .on('close', function () {
            checkEqual(savePath, captures.small.path);
            done();
          });

      });

      it('can be piped to and infer the link type', function (done) {

        var savePath = fromName('pipe_infer.pcap');
        var replay = new dot11.capture.Replay(captures.small.path);
        var save = new Save(savePath);

        replay
          .pipe(save)
          .on('close', function () {
            checkEqual(savePath, captures.small.path);
            done();
          });

      });

      it('truncates packets if necessary', function (done) {

        var savePath = fromName('truncate.pcap');
        var replay = new dot11.capture.Replay(captures.small.path);
        var save = new Save(savePath, {maxPacketSize: 50});

        replay
          .pipe(save)
          .on('close', function () {
            new dot11.capture.Replay(savePath)
              .on('data', function (buf) { assert.ok(buf.length <= 50); })
              .on('error', function () {}) // Skip packet overflow errors.
              .on('end', function () { done(); });
          });

      });

      it('closes after finish', function (done) {

        var savePath = fromName('close.pcap');
        var replay = new dot11.capture.Replay(captures.small.path);
        var save = new Save(savePath, {linkType: replay.getLinkType()});
        var finished = false;

        save
          .on('finish', function () { finished = true; replay.close(); })
          .on('close', function () { assert.ok(finished); done(); })
          .end(replay.read());

      });

      // Helpers.

      // Name and delete file after test.
      function fromName(fname) {

        var savePath = path.join(__dirname, fname);
        after(function () { fs.unlink(savePath); });
        return savePath;

      }

      // Check that two replay files are equal.
      function checkEqual(pathA, pathB) {

        var replayA = new dot11.capture.Replay(pathA);
        var replayB = new dot11.capture.Replay(pathB);

        var a, b;
        while ((a = replayA.read()) !== null || (b = replayB.read()) !== null) {
          assert.deepEqual(a, b);
        }

      }

    });

    // Live capture (relies on having a functional interface).

    maybe(describe, hasActiveDevice())('Live', function () {

      var Live = dot11.capture.Live;

      beforeEach(function () {
        // Make sure there are some packets to listen to.

        setTimeout(randomRequest, 0);
        setTimeout(randomRequest, 50);

        function randomRequest() {
          var url = 'http://' + crypto.randomBytes(20).toString('hex') + '.com';
          http.get(url).on('error', function () {});
        }

      });

      it('can find the default device', function () {
        // We already know this will work but oh well.

        var dev = Live.getDefaultDevice();
        assert.ok(dev !== null);

      });

      it('reads a single packet', function (done) {

        new Live()
          .once('readable', function () {
            var data = this.read();
            var stats = this.getStats();
            this.close();
            assert.ok(data !== null);
            assert.ok(stats.psRecv > 0);
            done();
          });

      });

      it('throws an error on packet truncation by default', function (done) {

        var nErrors = 0;

        new Live(null, {maxPacketSize: 10})
          .close(500)
          .on('error', function () { nErrors++; })
          .on('data', function () {})
          .on('end', function () {
            assert.ok(nErrors > 0);
            done();
          });

      });

      it('emits events and closes', function (done) {

        var totalPackets = 2;
        var nPackets = 0;
        var stats;

        new Live()
          .on('data', function (data) {
            assert.ok(data !== null);
            if (++nPackets === totalPackets) {
              stats = this.getStats();
              this.close();
            }
          })
          .on('end', function () {
            assert.ok(nPackets >= totalPackets);
            assert.ok(stats && stats.psRecv >= 2);
            done();
          });

      });

      it('closes after a given timeout', function (done) {

        var nPackets = 0;

        new Live()
          .close(10)
          .on('data', function () { nPackets++; })
          .on('end', function () {
            assert.ok(nPackets > 0);
            done();
          });

      });

      it('closes from outside', function (done) {

        var nPackets = 0;
        var ended = false;
        var finished = false;
        var capture = new Live();

        capture
          .on('data', function () { nPackets++; })
          .on('finish', function () { finished = true; })
          .on('end', function () { ended = true; })
          .on('close', function () {
            assert.ok(nPackets > 0);
            assert.ok(finished);
            assert.ok(ended);
            done();
          });
        setTimeout(function () { capture.close(); }, 1);

      });

      it('closes after the writable side finishes', function (done) {

        var capture = new Live();
        var ended = false;
        var finished = false;

        capture
          .on('data', function () {})
          .on('finish', function () { finished = true; })
          .on('end', function () { ended = true; })
          .on('close', function () {
            assert.ok(ended);
            assert.ok(finished);
            done();
          });

        setTimeout(function () { capture.end(); }, 200);

      });

      it('closes after the readable side ends', function (done) {

        var capture = new Live();
        var ended = false;
        var finished = false;

        capture
          .on('data', function () {})
          .on('finish', function () { finished = true; })
          .on('end', function () { ended = true; })
          .on('close', function () {
            assert.ok(ended);
            assert.ok(finished);
            done();
          });

        setTimeout(function () { capture.push(null); }, 200);

      });

      it.skip('can inject a packet', function (done) {
        // TODO: fix this test.

        var capture = new Live();
        var packet = new Buffer('000019006f08000066be02f80000000012309e098004d2a400c4006e008438355f8e8a486fb74b', 'hex');
        var found = false;

        capture
          .once('readable', function () {
            var i = 100;
            while (i--) {
              this.write(packet);
            }
          })
          .on('data', function (buf) {
            found = found || bufferEquals(buf, packet);
          })
          .on('end', function () {
            assert.ok(found);
            done();
          });
        setTimeout(function () { capture.close(); }, 2000);

      });

    });

  });

  // Global helpers.

  // Skip test if predicate is false (fn should be `describe` or `it`).
  function maybe(fn, predicate) {

    return predicate ? fn : fn.skip;

  }

  // Check whether two buffers are equal.
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

  function hasActiveDevice() {
    // Check whether there is a semi active network we can listen to.

    var device;
    try {
      device = dot11.capture.Live.getDefaultDevice();
    } catch (err) {
      device = null;
    }
    return !!device;

  }

})();
