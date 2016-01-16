/* jshint node: true, mocha: true */

'use strict';

var layer2 = require('../lib'),
    assert = require('assert'),
    path = require('path');


var DPATH = path.join(__dirname, 'dat'); // Test data directory.

// Empty for now!
suite('index', function () {

  test('read capture file', function (done) {
    var n = 0;
    layer2.createFileSniffer(path.join(DPATH, 'sample.pcap'))
      .on('pdu', function () { n++; })
      .on('end', function () {
        assert.equal(n, 10);
        done();
      });
  });

});
