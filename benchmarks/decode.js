/* jshint node: true */

(function () {
  'use strict';

  var util = require('util'),
      path = require('path'),
      tmp = require('tmp'),
      utils = require('./utils'),
      layer2 = require('../src/js'),
      pcap = require('pcap');

  tmp.setGracefulCleanup();
  pcap.decode.logicalLinkControl = function () {}; // Patch this.

  var benchmark = new utils.Benchmark()
    .addFn('layer2', function (cb, opts) {
      var nFrames = 0;
      var capture = new layer2.capture.Replay(opts.fpath);
      var decoder = new layer2.Decoder();
      capture
        .pipe(decoder)
        .on('data', function (frame) {
          // if (!nFrames) { console.dir(frame); }
          nFrames++;
        })
        .on('end', function () { cb(); });
    })
    .addFn('layer2 no-crc', function (cb, opts) {
      var nFrames = 0;
      var capture = new layer2.capture.Replay(opts.fpath);
      var decoder = new layer2.Decoder({assumeValid: true});
      capture
        .pipe(decoder)
        .on('data', function (frame) {
          // if (!nFrames) { console.dir(frame); }
          nFrames++;
        })
        .on('end', function () { cb(); });
    })
    .addFn('pcap', function (cb, opts) {
      var nFrames = 0;
      var session = pcap.createOfflineSession(opts.fpath);
      session
        .on('packet', function (frame) {
          // if (!nFrames) { console.dir(pcap.decode.packet(frame)); }
          nFrames++;
          pcap.decode.packet(frame);
        })
        .on('complete', function () { session.close(); cb(); });
    });

  // var fpaths = utils.getCapturePaths('dat/');
  var fpaths = ['../dat/valid.pcap'];
  var nRuns = 20;

  (function run(i) {
    if (i < fpaths.length) {
      var fpath = fpaths[i];
      var fname = path.basename(fpath, '.pcap');
      utils.expand(fpath, null, 5e7, function (err, summary, tpath) {
        benchmark.run(nRuns, {fpath: tpath}, function (stats) {
          console.log(util.format(
            '\n# %s (%s frames, %s runs)',
            fname, summary.nFrames, nRuns
          ));
          for (var j = 0; j < stats.length; j++) {
            var e = stats[j];
            console.log(util.format(
              '%s ms (±%s)\t %s f/us\t %s B/us\t%s\t%s',
              e.avgMs.toFixed(2),
              e.sdvMs.toFixed(2),
              (1e-3 * summary.nFrames / e.avgMs).toFixed(2),
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
    }
  })(0);

})();
