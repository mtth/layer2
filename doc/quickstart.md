# Quickstart

## Capture

In the example below, we create a
[`Live`](https://github.com/mtth/dot11/blob/master/doc/api.md#class-dot11capturelive)
stream from the default network interface and store 5 seconds' worth of frames to
a file by writing to a
[`Save`](https://github.com/mtth/dot11/blob/master/doc/api.md#class-dot11capturesave)
stream. We also print each frame's length and the total number of frames
processed at the very end.

```javascript
var dot11 = require('dot11');

var liveStream = new dot11.capture.Live(); // Readable stream of frames.
var saveStream = new dot11.capture.Save('log.pcap'); // Writable stream.

liveStream
  .close(5000)
  .on('data', function (buf) { console.log('Read frame of length: ' + buf.length); })
  .on('end', function () { console.log('Read ' + this.getStats().psRecv + ' frames!'); })
  .pipe(saveStream);
```

The above example is only using the default options, it is possible to specify
which interface to listen to and what modes to activate (e.g. promiscuous or
monitor). A
[`Replay`](https://github.com/mtth/dot11/blob/master/doc/api.md#class-dot11capturereplay)
stream is also available to stream frames from a saved capture file.
