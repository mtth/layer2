/* jshint node: true */

(function (root) {
  'use strict';

  var dot11 = require('../src/js'),
      fs = require('fs'),
      path = require('path');

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
        });

    })();

  }

  /**
   * Benchmark functions serially.
   *
   * This is useful for polite functions (which yield too often to the event
   * loop to be correctly benchmarked by traditional async code).
   *
   */
  function Benchmark() {

    var fns = {};

    /**
     * Add a function to be benchmarked.
     *
     * @param `name` A name to identify this function by.
     * @param `fn(cb, opts)` The function to be benchmarked. This function will
     * be passed two arguments: `cb([time])`, to be called when the function
     * completes (optionally passing an `hrtime` argument to override the
     * default time, e.g. to bypass setup and teardown durations), and the same
     * `opts` argument that was passed to the benchmark's `run` method.
     * @return `this`
     *
     */
    this.addFn = function (name, fn) {

      fns[name] = fn;
      return this;

    };

    /**
     * Run all functions serially.
     *
     * @param `n` The number of runs per function.
     * @param `opts` Arguments to be passed to each benchmarked function (see
     * `addFn`).
     * @param `cb(stats)` Callback when the run completes.
     * @return `this`
     *
     */
    this.run = function (n, opts, cb) {

      if (!cb && typeof opts == 'function') {
        cb = opts;
        opts = {};
      }

      var names = Object.keys(fns);
      var times = {};
      var i = 0;

      (function runCb(ts) {
        if (ts) {
          times[names[i++]] = ts;
        }
        if (i < names.length) {
          var fn = fns[names[i]];
          runFn(fn, n, opts, runCb);
        } else {
          cb(times);
        }
      })();

      return this;

    };

    function runFn(fn, n, opts, cb) {

      var times = [];
      var i = 0;
      var t;

      (function runCb(time) {
        if (i) {
          times.push(time || process.hrtime(t));
        }
        if (i++ < n) {
          t = process.hrtime();
          fn(runCb, opts);
        } else {
          cb(getStats(times));
        }
      })();

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
        var sdvMs = Math.sqrt(varMs) / ts.length;
        return {avgMs: avgMs, sdvMs: sdvMs};
      }

    }

  }

  /**
   * Get all saved captures.
   *
   * @return The list of file paths.
   *
   */
  function getCapturePaths(dpath) {

    var fnames = fs.readdirSync(dpath);
    var fpaths = [];
    for (var i = 0; i < fnames.length; i++) {
      fpaths.push(path.join(dpath, fnames[i]));
    }
    return fpaths;

  }

  root.exports = {
    Benchmark: Benchmark,
    expand: expand,
    getCapturePaths: getCapturePaths
  };

})(module);
