/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var capture = require('../lib/capture'),
      assert = require('assert'),
      crypto = require('crypto'),
      fs = require('fs'),
      path = require('path'),
      http = require('http');

  describe('Capture', function () {

    var savefilePath = path.join(__dirname, 'dat/mesh.pcap');

    it('can summarize saved captures', function (done) {

      capture.summarize(savefilePath, function (err, summary) {
        assert.deepEqual(summary, {
          linkType: 127,
          maxFrameSize: 65535,
          nFrames: 780,
          nBytes: 117469 // TODO: check this number
        });
        done();
      });

    });

    // Replays.

    describe('Replay', function () {

      var Replay = capture.Replay;

      it('returns the correct maximum frame size', function () {

        var capture = new Replay(savefilePath);
        assert.equal(capture.getMaxFrameSize(), 65535);

      });

      it('returns the correct link type', function () {

        var capture = new Replay(savefilePath);
        assert.equal(capture.getLinkType(), 127);

      });

      it('returns the correct path', function () {

        var capture = new Replay(savefilePath);
        assert.equal(capture.getPath(), savefilePath);

      });

      it('can read an entire file', function (done) {

        var nFrames = 0;

        new Replay(savefilePath)
          .on('data', function () { nFrames++; })
          .on('end', function () {
            assert.equal(nFrames, 780);
            done();
          });

      });

      it('can be read by several handlers', function (done) {

        var nFrames1 = 0;
        var nFrames2 = 0;
        var nFrames3 = 0;

        new Replay(savefilePath)
          .on('data', function () { nFrames1++; })
          .on('data', function () { nFrames2++; })
          .on('data', function () { nFrames3++; })
          .on('end', function () {
            assert.equal(nFrames1, 780);
            assert.equal(nFrames2, 780);
            assert.equal(nFrames3, 780);
            done();
          });

      });

      it('can be read frame by frame', function (done) {

        var frames = [];

        new Replay(savefilePath)
          .on('readable', function () {
            var frame;
            while ((frame = this.read()) !== null) {
              frames.push(frame);
            }
          })
          .on('end', function () {
            assert.equal(frames.length, 780);
            done();
          });

      });

      it('supports filters', function (done) {

        var nFrames = 0;
        var filter = 'arp';

        new Replay(savefilePath, {filter: filter})
          .on('data', function () { nFrames++; })
          .on('end', function () {
            assert.equal(nFrames, 131); // 131 ARP packets in file.
            done();
          });

      });

    });

    // Saves.

    describe('Save', function () {

      var Save = capture.Save;

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
        var replay = new capture.Replay(savefilePath);
        var save = new Save(savePath, {
          linkType: replay.getLinkType()
        });

        replay
          .on('data', function (frame) { save.write(frame); })
          .on('end', function () { save.end(); }); // Not piping so manual close.

        save
          .on('finish', function () {
            checkEqual(this.getPath(), savefilePath, done);
          });


      });

      it('can be piped to', function (done) {

        var savePath = fromName('pipe.pcap');
        var replay = new capture.Replay(savefilePath);
        var save = new Save(savePath, {linkType: replay.getLinkType()});

        replay
          .pipe(save)
          .on('finish', function () {
            checkEqual(savePath, savefilePath, done);
          });

      });

      it('can be piped to and infer the link type', function (done) {

        var savePath = fromName('pipe_infer.pcap');
        var replay = new capture.Replay(savefilePath);
        var save = new Save(savePath);

        replay
          .pipe(save)
          .on('finish', function () {
            checkEqual(savePath, savefilePath, done);
          });

      });

      it('truncates frames if necessary', function (done) {

        var savePath = fromName('truncate.pcap');
        var replay = new capture.Replay(savefilePath);
        var save = new Save(savePath, {maxFrameSize: 50});

        replay
          .pipe(save)
          .on('finish', function () {
            new capture.Replay(savePath)
              .on('data', function (frame) { assert.ok(frame.size() <= 50); })
              .on('error', function () {}) // Skip frame overflow errors.
              .on('end', function () { done(); });
          });

      });

      it('can be piped to multiple times', function (done) {

        var loop = 3;
        var savePath = fromName('multiple.pcap');
        var save = new Save(savePath);

        save
          .on('finish', function () {
            capture.summarize(savePath, function (err, summary) {
              assert.equal(summary.nFrames, 780 * 3);
              done();
            });
          });

        (function saveOnce() {

          new capture.Replay(savefilePath)
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
      function checkEqual(pathA, pathB, cb) {

          var replayA = new capture.Replay(pathA);
          var replayB = new capture.Replay(pathB);

          var a, b;
          while ((a = replayA.read()) !== null || (b = replayB.read()) !== null) {
            assert.deepEqual(a, b);
          }
          cb();

      }

    });

    // Live capture (relies on having a functional interface).

    maybe(describe, hasActiveDevice())('Live', function () {

      var Live = capture.Live;
      var dev = capture.getDefaultDevice();

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
            assert.ok(data !== null);
            assert.ok(stats.psRecv > 0);
            done();
          });

      });

      it('emits finish events when ended', function (done) {

        var totalFrames = 2;
        var nFrames = 0;
        var stats;

        new Live(dev)
          .on('data', function (data) {
            assert.ok(data !== null);
            if (++nFrames === totalFrames) {
              stats = this.getStats();
              this.end();
            }
          })
          .on('finish', function () {
            assert.ok(nFrames >= totalFrames);
            assert.ok(stats && stats.psRecv >= 2);
            done();
          });

      });

      it('can be ended from outside', function (done) {

        var nFrames = 0;
        var capture = new Live(dev);

        capture
          .on('data', function () { nFrames++; })
          .on('finish', function () {
            assert.ok(nFrames > 0);
            done();
          });
        setTimeout(capture.end.bind(capture), 250);

      });

      it('supports filters', function () {

        var filter = 'ether host 01:02:03:04:05:06';
        new Live(dev, {filter: filter}).end();

      });

      it('is consistent accross captures', function (done) {

        var frames1 = [];
        var frames2 = [];

        var onFinish = (function () {
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

        var l1 = new Live(dev)
          .on('finish', onFinish)
          .on('data', function (buf) { frames1.push(buf.toString('hex')); });

        var l2 = new Live(dev)
          .on('finish', onFinish)
          .on('data', function (buf) { frames2.push(buf.toString('hex')); });

        setTimeout(l1.end.bind(l1), 1000);
        setTimeout(l2.end.bind(l2), 1000);

      });

      it('can be opened many times', function (done) {

        var nCaptures = 20;
        var frameCounts = [];

        var onFinish = (function () {
          var nClosed = 0;
          return function () {
            if (++nClosed === nCaptures) {
              assert.ok(Math.min.apply(null, frameCounts) > 0);
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
          var live = new Live(dev)
            .on('finish', onFinish)
            .on('data', function () { frameCounts[i]++; });
          setTimeout(live.end.bind(live), 1000);
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
      device = capture.getDefaultDevice();
    } catch (err) {
      device = null;
    }
    return !!device;

  }

})();
