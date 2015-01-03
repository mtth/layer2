/* jshint node: true */

(function () {
  'use strict';

  var fs = require('fs'),
      path = require('path'),
      util = require('util'),
      bluebird = require('bluebird'),
      dot11 = require('../src/js'),
      pcap = require('pcap'),
      tmp = require('tmp'),
      stream = require('stream'),
      PcapStream = require('pcap-stream');

  /**
   * Expand replay to given number of bytes.
   *
   * The final size might be slightly under than the input size (less than the
   * maximum frame size). Note that the entire file's size might be a bit over
   * because of the global header.
   *
   * @param srcPath Source.
   * @param dstPath Target.
   * @param cb(err, summary)
   *
   */
  function expand(srcPath, dstPath, totalBytes, cb) {

    var nBytes = 0;
    var save = null;

    (function loop() {

      var finished = false;

      new dot11.capture.Replay(srcPath)
        .once('readable', function () {
          if (save === null) {
            save = new dot11.capture.Save(dstPath, {
              linkType: this.getLinkType()
            })
              .on('close', function () {
                util.debug('closing');
                if (cb) {
                  dot11.capture.summarize(dstPath, cb);
                }
              });
          }
        })
        .on('data', function (buf) {
          nBytes += buf.length;
          if (nBytes < totalBytes) {
            save.write(buf);
          } else {
            if (!finished) {
              finished = true;
              this.close();
              save.end();
            }
          }
        })
        .on('end', function () {
          if (!finished) {
            loop(save);
          }
        })

    })();

  }

  /**
   * Benchmark functions serially.
   *
   */
  function Benchmark() {

    var fns = {};

    this.addFn = function (name, fn) {

      fns[name] = fn;
      return this;

    };

    this.run = function (n, cb) {

      var times = {};
      var names = Object.keys(fns);
      var i = 0;

      runFn(fns[names[0]], n, runCb);

      return this;

      function runCb(ts) {
        times[names[i++]] = ts;
        if (i < names.length) {
          var fn = fns[names[i]];
          runFn(fn, n, runCb);
        } else {
          cb(times);
        }
      }

    }

    function runFn(fn, n, cb) {

      var times = [];

      fn(runCb);

      function runCb(time) {
        times.push(time);
        if (--n) {
          fn(runCb);
        } else {
          cb(getStats(times));
        }
      }

      function getStats(ts) {
        var totMs = 0;
        for (var i = 0; i < ts.length; i++) {
          var time = ts[i];
          totMs += 1e3 * time[0] + 1e-6 * time[1];
        }
        var avgMs = totMs / ts.length;
        var varMs = 0;
        for (i = 0; i < ts.length; i++) {
          time = ts[i];
          varMs += Math.pow(1e3 * time[0] + 1e-6 * time[1] - avgMs, 2);
        }
        var sdvMs = Math.sqrt(varMs);
        debugger;
        return {avgMs: avgMs, sdvMs: sdvMs};
      }

    }

  }

  var fpath = 'dat/4.pcap';

  new Benchmark()
    .addFn('dot11', function (cb) {
      var t = process.hrtime();
      var nFrames = 0;
      var capture = new dot11.capture.Replay(fpath);
      capture
        .on('data', function () { nFrames++; })
        .on('end', function () { cb(process.hrtime(t)); });
    })
    .addFn('pcap', function (cb) {
      var t = process.hrtime();
      var nFrames = 0;
      var session = pcap.createOfflineSession(fpath);
      session
        .on('packet', function () { nFrames++; })
        .on('complete', function () { cb(process.hrtime(t)); });
    })
    .addFn('pcap-stream', function (cb) {
      var t = process.hrtime();
      var nFrames = 0;
      var pStream = new PcapStream(fpath);
      pStream
        .on('data', function () { nFrames++; })
        .on('end', function () { cb(process.hrtime(t)); });
    })
    .run(5, function (ts) {
      console.dir(ts);
    });

})();
