/* jshint node: true, mocha: true */

'use strict';

var sniffers = require('../lib/sniffers'),
    utils = require('../lib/utils'),
    assert = require('assert'),
    path = require('path');


var DPATH = path.join(__dirname, 'dat'); // Test data directory.

suite('sniffers', function () {

  suite('Sniffer', function () {

    var type; // PDU type, loaded asynchronously.

    before(function (done) {
      utils.loadPduType(function (err, type_) {
        type = type_;
        done();
      });
    });

    test('eager single PDU', function (done) {
      var val = type.random();
      var w = new Wrapper([val]);
      new sniffers.Sniffer(w)
        .on('pdu', function (pdu) {
          assert(!pdu.compare(val));
          this.destroy();
        })
        .on('end', function () {
          assert(w.destroyed);
          done();
        });
    });

    test('delayed single PDU', function (done) {
      var val = type.random();
      var w = new Wrapper([val]);
      var s = new sniffers.Sniffer(w);
      setImmediate(function () {
        s.on('pdu', function (pdu) {
          assert(!pdu.compare(val));
          done();
        });
      });
    });

    // Mock wrapper to test sniffer logic.
    function Wrapper(pdus) {
      this._pdus = pdus;
      this.destroyed = false;
    }

    Wrapper.prototype.getPdus = function (buf, cb) {
      var pdus = this._pdus;
      var n = 0;
      var pos = 0;
      while (pdus.length && (pos = type.encode(pdus[0], buf, pos)) > 0) {
        n++;
        pdus.shift();
      }
      setImmediate(function () { cb(null, n); });
    };

    Wrapper.prototype.destroy = function () { this.destroyed = true; };

  });

  suite('file sniffer', function () {

    test('missing file', function () {
      assert.throws(function () {
        sniffers.createFileSniffer(path.join(DPATH, 'foo.pcap'));
      }, /No such file/);
    });

    test('non empty', function (done) {
      var n = 0;
      sniffers.createFileSniffer(path.join(DPATH, 'sample.pcap'))
        .on('pdu', function () { n++; })
        .on('end', function () {
          assert.equal(n, 10);
          done();
        });
    });

  });

});
