/* jshint node: true */

/**
 * Benchmark various MAC address parsing methods.
 *
 */
(function () {
  'use strict';

  var utils = require('./utils'),
      addon = require('../src/js/utils').requireAddon();

  var benchmark = new utils.Benchmark();

  benchmark
    .addFn('radiotap', function (done) {

      var buf = new Buffer('000020006708040054c6b82400000000220cdaa002000000400100003c142411b4007c013ce072e6612bcc03fadc202a719fe3d6', 'hex');
      var i = 1000;
      var frame, pdu;
      var t = 0;

      while (i--) {
        frame = new addon.Frame(127, buf);
        pdu = frame.getPdu(3);
        t += pdu.getChannel().freq;
      }
      done();

    })
    .run(1e2, function (stats) {
      console.dir(stats);
    });

})();
