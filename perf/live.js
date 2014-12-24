/* jshint mocha: true, node: true */

/**
 * Live performance tests.
 *
 */
(function () {
  'use strict';

  var dot11 = require('../dot11'),
      assert = require('assert');

  describe('Live capture', function () {

    var device = 'en0';
    var opts = {monitor: true, promisc: true}; // Speed up.

    it('reads a single packet', function (done) {

      new dot11.capture.Live(device, opts)
        .once('readable', function () {
          var data = this.read();
          var stats = this.getStats();
          this.close();
          assert.ok(data !== null);
          assert.ok(stats.psRecv > 0);
          done();
        });

    });

    it('emits events and closes', function (done) {

      var totalPackets = 10;
      var nPackets = 0;
      var stats;

      new dot11.capture.Live(device, opts)
        .on('data', function (data) {
          assert.ok(data !== null);
          if (++nPackets === totalPackets) {
            stats = this.getStats();
            this.close();
          }
        })
        .on('end', function () {
          assert.ok(nPackets >= totalPackets);
          assert.ok(stats && stats.psRecv >= nPackets);
          done();
        });

    });

    it('lets the event loop run once in a while', function (done) {

      var capture = new dot11.capture.Live(device, opts);
      var nPackets = 0;

      capture
        .on('data', function () { nPackets++; })
        .on('end', function () { assert.ok(nPackets > 0); done(); });
      setTimeout(function () { capture.close(); }, 500);

    });

  });

})();
