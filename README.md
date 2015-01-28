# Layer2 [![Build Status](https://travis-ci.org/mtth/layer2.svg?branch=master)](https://travis-ci.org/mtth/layer2)

`layer2` leverages Node.js' built-in [streams][] to provide an intuitive and
efficient interface to the [data link layer][wiki]. It supports frame capture
and injection, and provides decoders for various link types.

```javascript
var layer2 = require('layer2');

var capture = new layer2.capture.Live('en0', {monitor: true});
var decoder = new layer2.Decoder();

capture           // Stream of buffers (frames' raw bytes).
  .pipe(decoder)  // Stream of decoded frames.
  .on('data', function (frame) { console.log(frame); });
```


## Installation

```bash
$ npm install layer2
```

`layer2` leverages [libpcap][] to capture frames and [libtins][] to decode
them. You'll need to install them first.


## Documentation

[API docs](doc/api.md) and several [examples](examples/) are available.


## Benchmarks

`layer2` is built with both ease of use and performance in mind. This means you
can benefit from the productivity gains of writing JavaScript and still handle
throughput rates of several hundred thousand frames per second (upwards of a
million when streaming raw frames). You can read more about this on the
[benchmarks page](doc/benchmarks.md).


## Tests

```bash
$ npm test
```

Some tests require an active internet connection.


[wiki]: http://en.wikipedia.org/wiki/Data_link_layer
[Radiotap]: http://www.radiotap.org/
[streams]: http://nodejs.org/api/stream.html
[libpcap]: http://www.tcpdump.org/
[libtins]: http://libtins.github.io/
