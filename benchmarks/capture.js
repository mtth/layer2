/* jshint node: true */

(function () {
  'use strict';

  var util = require('util'),
      tmp = require('tmp'),
      utils = require('./utils'),
      dot11 = require('../src/js'),
      pcap = require('pcap'),
      PcapStream = require('pcap-stream');

  tmp.setGracefulCleanup();

  var benchmark = new utils.Benchmark()
    .addFn('dot11', function (cb, opts) {
      var nFrames = 0;
      var capture = new dot11.capture.Replay(opts.fpath);
      capture
        .on('data', function () { nFrames++; })
        .on('end', function () { cb(); });
    })
    .addFn('pcap', function (cb, opts) {
      var nFrames = 0;
      var session = pcap.createOfflineSession(opts.fpath);
      session
        .on('packet', function () { nFrames++; })
        .on('complete', function () { session.close(); cb(); });
    })
    .addFn('pcap-stream', function (cb, opts) {
      var nFrames = 0;
      var pStream = new PcapStream(opts.fpath);
      pStream
        .on('data', function () { nFrames++; })
        .on('end', function () { cb(); });
    });

  var fpaths = utils.getCapturePaths('dat/');

  (function run(i) {
    if (i < fpaths.length) {
      var fpath = fpaths[i];
      utils.expand(fpath, null, 5e7, function (err, summary, tpath) {
        benchmark.run(20, {fpath: tpath}, function (stats) {
          console.log('\n# ' + fpath + ' (' + summary.nFrames + ' frames)');
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
