/* jshint node: true */

(function () {
  'use strict';

  var fs = require('fs'),
      http = require('http'),
      path = require('path'),
      dot11 = require('../lib');

  var captures = [
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=mesh.pcap',
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=airtunes-1.pcap',
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=dhcp.pcap',
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=vnc-sample.pcap',
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=slammer.pcap',
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=dns-remoteshell.pcap',
    'http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=arp-storm.pcap'
  ];

  var dPath = path.join(__dirname, 'dat');

  setup(1000);

  // Download and expand (by n) capture files.
  function setup(n) {

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

    // Duplicate packets from a capture.
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

})();
