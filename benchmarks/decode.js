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

  (function patchPcap() {

    var decode = pcap.decode;

    decode.logicalLinkControl = function () {};

    decode.packet = function (raw_packet) {
      var packet = {};
      packet.link_type = raw_packet.pcap_header.link_type;
      switch (packet.link_type) {
        case "LINKTYPE_ETHERNET":
        packet.link = decode.ethernet(raw_packet, 0);
        break;
        case "LINKTYPE_NULL":
        packet.link = decode.nulltype(raw_packet, 0);
        break;
        case "LINKTYPE_RAW":
        packet.link = decode.rawtype(raw_packet, 0);
        break;
        case "LINKTYPE_IEEE802_11_RADIO":
        packet.link = decode.ieee802_11_radio(raw_packet, 0);
        break;
        case "LINKTYPE_LINUX_SLL":
        packet.link = decode.linux_sll(raw_packet, 0);
        break;
        case "Unknown linktype 105": // Ha.
        packet.link = decode.ieee802_11_frame(raw_packet, 0);
        break;
        default:
        console.log("pcap.js: decode.packet() - Don't yet know how to decode link type " + raw_packet.pcap_header.link_type);
      }
      packet.pcap_header = raw_packet.pcap_header;
      return packet;
    };

  })();

  var benchmark = new utils.Benchmark()
    .addFn('layer2', function (cb, opts) {
      var nFrames = 0;
      var nInvalidFrames = 0;
      var capture = new layer2.capture.Replay(opts.fpath);
      var decoder = new layer2.Decoder();
      capture
        .pipe(decoder)
        .on('data', function () { nFrames++; })
        .on('invalid', function () { nInvalidFrames++; })
        .on('end', function () {
          // console.log('valid: ' + nFrames + ' invalid: ' + nInvalidFrames);
          cb();
        });
    })
    .addFn('layer2 no-crc', function (cb, opts) {
      var nFrames = 0;
      var nInvalidFrames = 0;
      var capture = new layer2.capture.Replay(opts.fpath);
      var decoder = new layer2.Decoder({assumeValid: true});
      capture
        .pipe(decoder)
        .on('data', function () { nFrames++; })
        .on('invalid', function () { nInvalidFrames++; })
        .on('end', function () {
          // console.log('valid: ' + nFrames + ' invalid: ' + nInvalidFrames);
          cb();
        });
    });
    // .addFn('pcap', function (cb, opts) {
    //   var nFrames = 0;
    //   var session = pcap.createOfflineSession(opts.fpath);
    //   session
    //     .on('packet', function (frame) {
    //       nFrames++;
    //       pcap.decode.packet(frame);
    //     })
    //     .on('complete', function () { session.close(); cb(); });
    // });

  var fpaths = utils.getCapturePaths('dat/');
  var benchmarkStats = {};
  var nRuns = 20;

  (function run(i) {
    if (i < fpaths.length) {
      var fpath = fpaths[i];
      var fname = path.basename(fpath, '.pcap');
      utils.expand(fpath, null, 5e7, function (err, summary, tpath) {
        if (layer2.Decoder.isSupported(summary.linkType)) {
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
        } else {
          process.nextTick(function () { run(i + 1); });
        }
      });
    } else {
      console.log('\nHighcharts graph data:');
      console.log(JSON.stringify(utils.toHighcharts(benchmarkStats, {
        title: 'Decoding',
        yAxisName: 'Frames / second',
        converter: function (o) { return 1e6 * o.frameThroughput; }
      })));
    }
  })(0);

})();
