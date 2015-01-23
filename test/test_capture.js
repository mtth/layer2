/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var layer2 = require('../src/js'),
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

    it('can summarize saved captures', function (done) {

      layer2.capture.summarize(captures.large.path, function (err, summary) {
        assert.deepEqual(summary, {
          linkType: 'IEEE802_11_RADIO',
          maxFrameSize: 65535,
          nFrames: 780,
          nBytes: 118675
        });
        done();
      });

    });

    // Replays.

    describe('Replay', function () {

      var Replay = layer2.capture.Replay;

      it('returns the correct maximum frame size', function () {

        var capture = new Replay(captures.large.path);
        assert.equal(capture.getMaxFrameSize(), 65535);

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

        var nFrames = 0;

        new Replay(captures.large.path)
          .on('data', function () { nFrames++; })
          .on('end', function () {
            assert.equal(nFrames, captures.large.length);
            done();
          });

      });

      it('can read an entire file and breaks if necessary', function (done) {

        var nFrames = 0;
        var nBreaks = 0;

        new Replay(captures.large.path, {
          bufferSize: 100000 // Not big enough for an entire batch.
        })
          .on('break', function () { nBreaks++; })
          .on('data', function () { nFrames++; })
          .on('end', function () {
            assert.equal(nFrames, captures.large.length);
            assert.ok(nBreaks > 0);
            done();
          });

      });

      it('breaks each time if the buffer size is small', function (done) {
        // I.e. equal to the max frame size.

        var nFrames = 0;
        var nBreaks = 0;

        new Replay(captures.large.path, {
          bufferSize: 65535, // Not big enough for an entire batch.
          maxFrameSize: 65535
        })
          .on('break', function () { nBreaks++; })
          .on('data', function () { nFrames++; })
          .on('end', function () {
            assert.equal(nFrames, captures.large.length);
            assert.equal(nBreaks, nFrames);
            done();
          });

      });

      it('can be read by several handlers', function (done) {

        var nFrames1 = 0;
        var nFrames2 = 0;
        var nFrames3 = 0;

        new Replay(captures.large.path)
          .on('data', function () { nFrames1++; })
          .on('data', function () { nFrames2++; })
          .on('data', function () { nFrames3++; })
          .on('end', function () {
            assert.equal(nFrames1, captures.large.length);
            assert.equal(nFrames2, captures.large.length);
            assert.equal(nFrames3, captures.large.length);
            done();
          });

      });

      it('can be read frame by frame', function (done) {

        var frames = [];

        new Replay(captures.small.path)
          .on('readable', function () {
            var buf;
            while ((buf = this.read()) !== null) {
              frames.push(buf);
            }
          })
          .on('end', function () {
            assert.equal(frames.length, captures.small.length);
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

      it('closes after ending', function (done) {

        var ended = false;

        new Replay(captures.large.path)
          .on('data', function () {})
          .on('end', function () { ended = true; })
          .on('close', function () { done(); });

      });

      it('can read a frame when the batch size is 1', function (done) {

        testReadSingleFrame(1, done);

      });

      it('can read a frame when the batch size is small', function (done) {
        // I.e. batch size smaller than frames in the file.

        testReadSingleFrame(2, done);

      });

      it('can read a frame when the batch size matches', function (done) {
        // I.e. batch size equal to frames in the file.

        testReadSingleFrame(3, done);

      });

      it('can read a frame when the batch size is large', function (done) {
        // I.e. batch size greater than frames in the file.

        testReadSingleFrame(4, done);

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

      it('yields correct frames', function (done) {

        var index = 0;
        var frames = {
          0: new Buffer('000020006708040054c6b82400000000220cdaa002000000400100003c14241180000000ffffffffffff06037f07a01606037f07a016b0773a40cb260000000064000105000a667265656273642d617001088c129824b048606c030124050400010000072a5553202401112801112c01113001113401173801173c011740011795011e99011e9d011ea1011ea5011e200100dd180050f2020101000003a4000027a4000042435e0062322f00', 'hex'),
          189: new Buffer('00002000670804006d5d2725000000002230d9a001000000400100003c142411d40000000019e3d3535246e97687', 'hex'),
          218: new Buffer('0000200067080400b51e2a2500000000220cd7a001000000400100003c14241108020000ffffffffffff06037f07a0160016cbace5f95083aaaa0300000008004500009be957000040117b3c0a0000c00a0000ff0277027700870d3b663034652033206970703a2f2f31302e302e302e3139323a3633312f7072696e746572732f41646f626550444638202222202241646f62652050444620382e3022202241646f62652050444620333031362e31303222206a6f622d7368656574733d6e6f6e652c6e6f6e65206c656173652d6475726174696f6e3d3330300a', 'hex'),
          491: new Buffer('000020006708040059b76f2500000000220cd8a001000000400100003c14241188020000ffffffffffff00037f07a0160019e3d35352208f00002001011f540500000019e3d35352aaaa03000000080600010800060400010019e3d35352a9fef7000000000000004f673238', 'hex')
        };

        new layer2.capture.Replay(captures.large.path)
          .on('data', function (data) {
            var original = frames[index++];
            if (original) {
              assert.ok(bufferEquals(data, original));
            }
          })
          .on('end', function () { done(); });

      });

      it('supports filters', function (done) {

        var nFrames = 0;
        var filter = 'arp';

        new Replay(captures.large.path, {filter: filter})
          .on('data', function () { nFrames++; })
          .on('end', function () {
            assert.equal(nFrames, 131); // 131 ARP packets in file.
            done();
          });

      });

      // Helpers.

      function testDispatching(batchSize, callback) {

        var totalFrames = captures.large.length;
        var totalFetches = Math.ceil(totalFrames / batchSize) + 1;
        var nFrames = 0;
        var nFetches = 0;

        new Replay(captures.large.path, {batchSize: batchSize})
          .on('data', function () { nFrames++; })
          .on('fetch', function (ratio) {
            nFetches++;
            if (nFetches < totalFetches - 1) assert.equal(ratio, 1);
            if (nFetches === totalFetches) assert.equal(ratio, 0);
          })
          .on('end', function () {
            assert.equal(nFrames, totalFrames);
            assert.equal(nFetches, totalFetches);
            callback();
          });

      }

      function testReadSingleFrame(batchSize, callback) {

        var readFrame = false;

        new Replay(captures.small.path, {batchSize: batchSize})
          .once('readable', function () {
            if (this.read() !== null) {
              readFrame = true;
            }
            this.on('data', function () {}); // Drain stream.
          })
          .on('end', function () {
            assert.ok(readFrame);
            callback();
          });

      }

    });

    // Saves.

    describe('Save', function () {

      var Save = layer2.capture.Save;

      it('returns the correct maximum frame size', function () {

        var savePath = fromName('max_frame_size_default.pcap');
        var save = new Save(savePath, {maxFrameSize: 1024});
        assert.equal(save.getMaxFrameSize(), 1024);

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
        var replay = new layer2.capture.Replay(captures.small.path);
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
        var replay = new layer2.capture.Replay(captures.small.path);
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
        var replay = new layer2.capture.Replay(captures.small.path);
        var save = new Save(savePath);

        replay
          .pipe(save)
          .on('close', function () {
            checkEqual(savePath, captures.small.path);
            done();
          });

      });

      it('truncates frames if necessary', function (done) {

        var savePath = fromName('truncate.pcap');
        var replay = new layer2.capture.Replay(captures.small.path);
        var save = new Save(savePath, {maxFrameSize: 50});

        replay
          .pipe(save)
          .on('close', function () {
            new layer2.capture.Replay(savePath)
              .on('data', function (buf) { assert.ok(buf.length <= 50); })
              .on('error', function () {}) // Skip frame overflow errors.
              .on('end', function () { done(); });
          });

      });

      it('closes after finish', function (done) {

        var savePath = fromName('close.pcap');
        var replay = new layer2.capture.Replay(captures.small.path);
        var save = new Save(savePath, {linkType: replay.getLinkType()});
        var finished = false;

        replay
          .pipe(save)
          .on('finish', function () { finished = true; })
          .on('close', function () { assert.ok(finished); done(); });

      });

      it('can be piped to multiple times', function (done) {

        var loop = 3;
        var savePath = fromName('multiple.pcap');
        var save = new Save(savePath);

        save
          .on('close', function () {
            layer2.capture.summarize(savePath, function (err, summary) {
              assert.equal(summary.nFrames, 9);
              done();
            });
          });

        (function saveOnce() {

          new layer2.capture.Replay(captures.small.path)
            .on('end', function () {
              if (--loop) {
                saveOnce(save);
              } else {
                save.end();
              }
            })
            .pipe(save, {end: false});

        })();

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

        var replayA = new layer2.capture.Replay(pathA);
        var replayB = new layer2.capture.Replay(pathB);

        var a, b;
        while ((a = replayA.read()) !== null || (b = replayB.read()) !== null) {
          assert.deepEqual(a, b);
        }

      }

    });

    // Live capture (relies on having a functional interface).

    maybe(describe, hasActiveDevice())('Live', function () {

      var Live = layer2.capture.Live;
      var dev = layer2.capture.getDefaultDevice();

      beforeEach(function () {
        // Make sure there are some frames to listen to.

        setTimeout(randomRequest, 0);
        setTimeout(randomRequest, 50);

        function randomRequest() {
          var url = 'http://' + crypto.randomBytes(20).toString('hex') + '.com';
          http.get(url).on('error', function () {});
        }

      });

      it('can find the default device', function () {
        // We already know this will work but oh well.

        assert.ok(dev !== null);

      });

      it('reads a single frame', function (done) {

        new Live(dev)
          .once('readable', function () {
            var data = this.read();
            var stats = this.getStats();
            this.close();
            assert.ok(data !== null);
            assert.ok(stats.psRecv > 0);
            done();
          });

      });

      it('throws an error on frame truncation by default', function (done) {

        var nErrors = 0;
        var nFrames = 0;

        new Live(dev, {maxFrameSize: 2})
          .close(500)
          .on('error', function () { nErrors++; })
          .on('data', function () { nFrames++; })
          .on('end', function () {
            assert.ok(nFrames === 0);
            assert.ok(nErrors > 0);
            done();
          });

      });

      it('emits events and closes', function (done) {

        var totalFrames = 2;
        var nFrames = 0;
        var stats;

        new Live(dev)
          .on('data', function (data) {
            assert.ok(data !== null);
            if (++nFrames === totalFrames) {
              stats = this.getStats();
              this.close();
            }
          })
          .on('end', function () {
            assert.ok(nFrames >= totalFrames);
            assert.ok(stats && stats.psRecv >= 2);
            done();
          });

      });

      it('supports closing after a few frames', function (done) {

        var nFrames = 0;

        new Live(dev, {batchSize: 1})
          .on('data', function () {
            if (nFrames++ === 10) this.close();
          })
          .on('end', function () {
            done();
          });

      });

      it('closes after a given timeout', function (done) {

        var nFrames = 0;

        new Live(dev)
          .on('data', function () { nFrames++; })
          .on('end', function () {
            assert.ok(nFrames > 0);
            done();
          })
          .close(500);

      });

      it('closes from outside', function (done) {

        var nFrames = 0;
        var ended = false;
        var finished = false;
        var capture = new Live(dev);

        capture
          .on('data', function () { nFrames++; })
          .on('finish', function () { finished = true; })
          .on('end', function () { ended = true; })
          .on('close', function () {
            assert.ok(nFrames > 0);
            assert.ok(finished);
            assert.ok(ended);
            done();
          });
        setTimeout(function () { capture.close(500); }, 1);

      });

      it('closes after the writable side finishes', function (done) {

        var capture = new Live(dev);
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

        var capture = new Live(dev);
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

      it('supports filters', function (done) {

        var filter = 'ether host 01:02:03:04:05:06';
        new Live(dev, {filter: filter})
          .close()
          .on('data', function () {})
          .on('close', done);

      });

      it('is consistent accross captures', function (done) {

        var frames1 = [];
        var frames2 = [];

        var onClose = (function () {
          var nClosed = 0;
          return function () {
            if (++nClosed === 2) {
              assert.ok(frames2.length);
              while (frames1.length && frames1[0] !== frames2[0]) {
                frames1.shift();
              }
              var length = Math.min(frames1.length, frames2.length);
              // We are guaranteed that 1 starts before 2, but not that 1 ends
              // before 2, so we must truncate to shortest.
              assert.ok(length);
              assert.deepEqual(
                frames1.slice(0, length),
                frames2.slice(0, length)
              );
              done();
            }
          };
        })();

        new Live(dev)
          .close(100)
          .on('close', onClose)
          .on('data', function (buf) { frames1.push(buf.toString('hex')); });

        new Live(dev)
          .close(100)
          .on('close', onClose)
          .on('data', function (buf) { frames2.push(buf.toString('hex')); });

      });

      it('can be opened many times', function (done) {

        var nCaptures = 50;
        var frameCounts = [];

        var onClose = (function () {
          var nClosed = 0;
          return function () {
            if (++nClosed === nCaptures) {
              assert.ok(Math.min.apply(null, frameCounts));
              done();
            }
          };
        })();

        var i;
        for (i = 0; i < nCaptures; i++) {
          frameCounts.push(0);
          createCapture(i);
        }

        function createCapture(i) {
          new Live(dev)
            .close(1000)
            .on('close', onClose)
            .on('data', function () { frameCounts[i]++; });
        }

      });

      it.skip('can inject a frame', function (done) {
        // TODO: fix this test. Two things potentially complicate this. First,
        // this obviously requires a network card that supports emitting raw
        // frames (it doesn't seem MacBook Airs do). Second, it doesn't seem
        // that frames emitted by the machine monitoring get captured (also
        // tested on a MacBook Air via Wireshark).

        var capture = new Live(dev);
        var frame = new Buffer('000019006f08000066be02f80000000012309e098004d2a400c4006e008438355f8e8a486fb74b', 'hex');
        var found = false;

        capture
          .once('readable', function () {
            var i = 100;
            while (i--) {
              this.write(frame);
            }
          })
          .on('data', function (buf) {
            found = found || bufferEquals(buf, frame);
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
      device = layer2.capture.getDefaultDevice();
    } catch (err) {
      device = null;
    }
    return !!device;

  }

})();
