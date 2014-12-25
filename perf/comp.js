/* jshint node: true */

/**
 * Comparing interfaces.
 *
 */
(function () {
  'use strict';

  var dot11 = require('../lib'),
      pcap = require('pcap'),
      cap = require('cap');

  // Drivers.

  var sampleCapturePath = process.argv[2];
  // TODO: error message if no argument or file doesn't exist.

  // Dot11
  function dot11Perf(name, dataCb, endCb) {

    var label = '[dot11] ' + name;
    console.time(label);
    var capture = new dot11.capture.Replay(sampleCapturePath, {
      // bufferSize: 10000,
      batchSize: 1000,
      highWaterMark: null
    });
    capture
      .on('data', function (data) { dataCb(data); })
      .on('break', function () { console.warn('breaking loop'); })
      .on('end', function () { endCb(); console.timeEnd(label); });

  }

  // Pcap
  function pcapPerf(name, dataCb, endCb) {

    var label = '[pcap]  ' + name;
    console.time(label);
    var session = pcap.createOfflineSession(sampleCapturePath);
    session
      .on('packet', function (data) { dataCb(data); })
      .on('complete', function () { endCb(); console.timeEnd(label); });

  }

  // Cap
  function capPerf(name, dataCb, endCb) {

    // TODO: but no offline mode currently.

  }


  // No processing test.

  function Increment() {
    var nPackets = 0;
    this.incr = function () { nPackets++; };
    this.get = function () { console.log(nPackets); };
  }
  var dot11Incr = new Increment();
  dot11Perf('incr', dot11Incr.incr, dot11Incr.get);
  // var pcapIncr = new Increment();
  // pcapPerf('incr', pcapIncr.incr, pcapIncr.get);

})();
