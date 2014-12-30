/* jshint node: true */

(function (root) {
  'use strict';

  var fs = require('fs'),
      http = require('http'),
      path = require('path'),
      dot11 = require('../../lib');

  var captures = [
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=mesh.pcap',
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=airtunes-1.pcap',
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=vnc-sample.pcap',
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=dns-remoteshell.pcap',
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=arp-storm.pcap',
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=wpa-Induction.pcap'
  ];

  if (require.main === module) {
    setup(1000);
  }

  // Download and expand (by n) capture files.
  function setup(n) {

    var dPath = path.join(__dirname, 'dat');

    fs.exists(dPath, function (exists) {

      if (exists) {
        downloadAll();
      } else {
        fs.mkdir(dPath, downloadAll);
      }

      function downloadAll() {
        var i, l;
        for (i = 0, l = captures.length; i < l; i++) {
          download(captures[i], function (fPath) {
            expand(fPath, n);
          });
        }
      }

    });

    // Download capture.
    function download(url, cb) {
      var name = /target=(.*)/.exec(url)[1];
      var fPath = path.join(dPath, name);
      console.log('Downloading ' + fPath);
      http.get(url, function (res) {
        res
          .pipe(fs.createWriteStream(fPath))
          .on('close', function () { cb(fPath); });
      });
    }

    // Duplicate frames from a capture.
    function expand(capturePath, n, cb) {
      var replay = new dot11.capture.Replay(capturePath);
      var save = new dot11.capture.Save(capturePath + '.' + n, {
        linkType: replay.getLinkType()
      });
      console.log('Expanding ' + capturePath);
      replay.on('data', function (buf) {
        var i = n;
        while (i--) {
          save.write(buf);
        }
      });
      if (cb) {
        replay.on('end', function () { cb(capturePath); });
      }
    }

  }

  // Display output
  function displayResults(label, time, nFrames, path) {

      var totalMs = time[0] * 1000 + time[1] / 1000000;
      var nsPerFrame = 1000000 * totalMs / nFrames;
      console.log(
        label + '\t' +
        nFrames + ' frames\t' +
        Math.floor(totalMs) + ' ms\t' +
        Math.floor(nsPerFrame) + ' ns/frame\t' +
        path
      );

  }

  root.exports = {
    displayResults: displayResults
  };

})(module);
