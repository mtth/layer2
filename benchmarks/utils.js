/* jshint node: true */

(function (root) {
  'use strict';

  var layer2 = require('../src/js'),
      fs = require('fs'),
      path = require('path'),
      tmp = require('tmp');

  /**
   * Expand replay to given number of bytes.
   *
   * The final size might be slightly under than the input size (less than the
   * maximum frame size). Note that the entire file's size might be a bit over
   * because of the global header.
   *
   * @param `srcPath` Source.
   * @param `dstPath` Target. If `null`, a temporary file will be created.
   * @param `cb(err, summary, dstPath)`.
   *
   */
  function expand(srcPath, dstPath, totalBytes, cb) {

    var nBytes = 0;
    var save = null;

    if (dstPath === null) {
      tmp.file(function (err, tpath) {
        if (err) {
          throw err;
        }
        loop(tpath);
      });
    } else {
      process.nextTick(function () { loop(dstPath); });
    }

    function loop(fpath) {

      var finished = false;

      new layer2.capture.Replay(srcPath)
        .on('error', function (err) { cb(err); })
        .on('data', function (buf) {
          if (save === null) {
            save = new layer2.capture.Save(fpath, {
              linkType: this.getLinkType()
            })
              .on('close', function () {
                if (cb) {
                  layer2.capture.summarize(fpath, function (err, summary) {
                    cb(err, summary, fpath);
                  });
                }
              });
          }
          nBytes += buf.length;
          if (nBytes < totalBytes) {
            save.write(buf);
          } else {
            if (!finished) {
              finished = true;
              save.end();
            }
          }
        })
        .on('end', function () {
          if (!finished) {
            loop();
          }
        });

    }

  }

  /**
   * Benchmark functions serially.
   *
   * This is useful for async functions which yield too often to the event
   * loop to be correctly benchmarked.
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
     * `run(n, [opts], cb)`
     *
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
      var stats = {};
      var i = 0;

      (function runCb(ts) {
        if (ts) {
          stats[names[i++]] = ts;
        }
        if (i < names.length) {
          setImmediate(function () { runFn(fns[names[i]], n, opts, runCb); });
        } else {
          cb(getRankedStats(stats));
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
          process.nextTick(function () { fn(runCb, opts); });
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

    function getRankedStats(stats) {

      var es = [];
      for (var name in stats) {
        var stat = stats[name];
        stat.name = name;
        es.push(stat);
      }

      es = es.sort(function (a, b) { return a.avgMs - b.avgMs; });

      var avg = es[0].avgMs;
      for (var i = 1; i < es.length; i++) {
        var e = es[i];
        e.relAvg = (e.avgMs - avg) / avg;
      }

      return es;

    }

  }

  /**
   * Highcharts friendly data.
   *
   * @param `stats` Object keyed by run name, with stats as values (as returned
   * by the benchmark's `run` method).
   *
   * This should be used with the basic column demo fiddle (link below), after
   * setting the height to 500px.
   *
   * http://jsfiddle.net/gh/get/jquery/1.9.1/highslide-software/highcharts.com/tree/master/samples/highcharts/demo/column-basic/
   *
   */
  function toHighcharts(benchmarkStats, opts) {

    opts = opts || {};
    var title = opts.title || null;
    var yAxisName = opts.yAxisName || null;
    var converter = opts.converter || function (o) { return o.avgMs; };

    var categories = Object.keys(benchmarkStats);
    var seriesObj = {};
    for (var i = 0; i < categories.length; i++) {
      var runStats = benchmarkStats[categories[i]];
      for (var j = 0; j < runStats.length; j++) {
        var fnStats = runStats[j];
        if (!(fnStats.name in seriesObj)) {
          seriesObj[fnStats.name] = [];
        }
        seriesObj[fnStats.name].push(converter(fnStats));
      }
    }

    var series = [];
    var fnNames = Object.keys(seriesObj).sort();
    for (i = 0; i < fnNames.length; i++) {
      var fnName = fnNames[i];
      series.push({name: fnName, data: seriesObj[fnName]});
    }

    return {
      chart: {type: 'column'},
      title: {text: title},
      legend: {
        layout: 'vertical',
        align: 'right',
        verticalAlign: 'top',
        floating: true,
        x: -5,
        y: 5,
        borderWidth: 1,
        backgroundColor: '#ffffff'
      },
      xAxis: {
        categories: categories,
        labels: {
          rotation: -45,
        }
      },
      yAxis: {
        min: 0,
        title: {
          text: yAxisName
        }
      },
      series: series
    };

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
    toHighcharts: toHighcharts,
    getCapturePaths: getCapturePaths
  };

})(module);
