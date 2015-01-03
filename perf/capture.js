/* jshint node: true */

(function () {
  'use strict';

  var fs = require('fs'),
      path = require('path'),
      util = require('util'),
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
      tmpExpand(fpath, 1e7, function (tpath, summary) {
        benchmark.run(20, {fpath: tpath}, function (ts) {
          console.log(fpath + ': ' + summary.nFrames + ' frames');
          console.dir(ts);
          run(i + 1);
        });
      });
    }
  })(0);

  function tmpExpand(fpath, nBytes, cb) {

    tmp.file(function (err, tpath) {

      if (err) {
        throw err;
      }

      utils.expand(fpath, tpath, nBytes, function (err, summary) {
        cb(tpath, summary);
      });

    });

  }

})();
