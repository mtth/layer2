/* jshint node: true */

(function () {
  'use strict';

  var util = require('util'),
      path = require('path'),
      tmp = require('tmp'),
      utils = require('./utils'),
      addon = require('../src/js/utils').requireAddon();

  tmp.setGracefulCleanup();

  var benchmark = new utils.Benchmark()
    .addFn('dispatch no headers', function (cb, opts) {
      var wrapper = new addon.PcapWrapper().fromSavefile(opts.fpath);
      var buf = new Buffer(1e7); // Large-ish.
      var prevOffset = 1; // Prime loop.
      var dispatchCb = function (offset) {
        var frame = buf.slice(prevOffset, offset);
        prevOffset = offset;
        processFrame(frame);
      };
      while (prevOffset) {
        prevOffset = 0;
        wrapper.dispatch(1000, buf, dispatchCb);
      }
      wrapper.close();
      cb();
    })
    .addFn('dispatch with headers', function (cb, opts) {
      var wrapper = new addon.PcapWrapper().fromSavefile(opts.fpath);
      var buf = new Buffer(1e7); // Large-ish.
      var prevOffset = 1; // Prime loop.
      var dispatchCb = function (offset) {
        var length = offset - prevOffset;
        var frame = {
          len: length,
          capLen: length,
          frame: buf.slice(prevOffset, offset)
        };
        prevOffset = offset;
        processFrame(frame);
      };
      while (prevOffset) {
        prevOffset = 0;
        wrapper.dispatch(1000, buf, dispatchCb);
      }
      wrapper.close();
      cb();
    })
    .addFn('fetch with headers', function (cb, opts) {
      var wrapper = new addon.PcapWrapper().fromSavefile(opts.fpath);
      var bufs = [new Buffer(1e6), new Buffer(1e6)];
      (function loop(bucket) {
        var buf = bufs[bucket];
        wrapper
          .fetch(-1, bufs[bucket], function (err, s, e, o) {

            if (e === 0) { // Flag call.
              if (s > 0) { // More frames still.
                loop(bucket ^ 1);
              } else {
                wrapper.close();
                cb();
              }
              return;
            }

            var h = {
              capLen: e - s,
              len: e - s,
              frame: buf.slice(s, e)
            };

            processFrame(h);

          });
      })(0);
    });

  var fpaths = utils.getCapturePaths('dat/');
  var benchmarkStats = {};
  var nRuns = 20;

  function processFrame(frame) {

    var i = 5;
    frame.things = [];
    while (i--) {
      frame.things.push(i);
    }

  }

  (function run(i) {
    if (i < fpaths.length) {
      var fpath = fpaths[i];
      var fname = path.basename(fpath, '.pcap');
      utils.expand(fpath, null, 5e7, function (err, summary, tpath) {
        benchmark.run(nRuns, {fpath: tpath}, function (stats) {
          benchmarkStats[fname] = stats;
          console.log(util.format(
            '\n# %s (%s frames, %s runs)',
            fname, summary.nFrames, nRuns
          ));
          for (var j = 0; j < stats.length; j++) {
            var e = stats[j];
            e.frameThroughput = 1e-3 * summary.nFrames / e.avgMs;
            console.log(util.format(
              '%s ms (±%s)\t %s f/us\t %s B/us\t%s\t%s',
              e.avgMs.toFixed(2),
              e.sdvMs.toFixed(2),
              e.frameThroughput.toFixed(2),
              (1e-3 * summary.nBytes / e.avgMs).toFixed(2),
              'relAvg' in e ?
                '-' + (100 / (1 + 1 / e.relAvg)).toFixed(0) + '%' :
                '',
              e.name
            ));
          }
          run(i + 1);
        });
      });
    } else {
      console.log('\nHighcharts graph data:');
      console.log(JSON.stringify(utils.toHighcharts(benchmarkStats, {
        title: 'Capture',
        yAxisName: 'Frames / second',
        converter: function (o) { return 1e6 * o.frameThroughput; }
      })));
    }
  })(0);

})();
