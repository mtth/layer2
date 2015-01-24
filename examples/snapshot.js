/* jshint node: true */

/**
 * Save a 5 second snapshot of live frames.
 *
 * We create a Live stream from the default network interface and store 5
 * seconds' worth of frames to a file by writing to a Save stream. We also
 * print the total number of frames processed at the very end.
 *
 */
(function () {
  'use strict';

  var layer2 = require('../src/js');

  if (process.argv.length != 3) {
    console.error('Usage: node snapshot.js PATH');
    process.exit(1);
  }

  var fpath = process.argv[2];
  var device = layer2.capture.getDefaultDevice();
  var liveStream = new layer2.capture.Live(device, {monitor: true});
  var saveStream = new layer2.capture.Save(fpath);

  liveStream
    .close(5000)
    .on('end', function () {
      console.log('Saved ' + this.getStats().psRecv + ' frames!');
    })
    .pipe(saveStream);

})();
