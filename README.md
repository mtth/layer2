# Level2 [![Node Version](https://img.shields.io/node/v/gh-badges.svg?style=flat)](https://www.npmjs.com/package/level2) [![Build Status](https://travis-ci.org/mtth/level2.svg?branch=master)](https://travis-ci.org/mtth/level2)

`level2` leverages Node.js' built-in [streams][] to provide an intuitive and
efficient interface for frame capture and injection. It also provides decoders
for various link types.

```javascript
var level2 = require('level2');

var capture = new level2.capture.Live('en0', {monitor: true});
var decoder = new level2.Decoder();

capture           // Stream of buffers (frames' raw bytes).
  .pipe(decoder)  // Stream of objects (decoded frames).
  .on('data', function (frame) { console.log(JSON.stringify(frame)); });
```


## Installation

```bash
$ npm install level2
```

`level2` depends on [libpcap][]. Note that you might also need to run live
captures as root.


## Documentation

[API docs](doc/api.md) and several [examples](examples/) are available.


## Benchmarks

`level2` is built with both ease of use and performance in mind. This means you
can benefit from the productivity gains of writing JavaScript and still handle
throughput rates higher than any network you will likely encounter.

The chart below gives an idea of the achievable raw frame throughput (also
compared to two other node capture libraries).

[![Capture benchmark](doc/img/capture.png)](doc/benchmarks.md#capture)

See [here](doc/benchmarks.md) for more details.


## Tests

```bash
$ npm test
```

Some tests require an active internet connection.


[Radiotap]: http://www.radiotap.org/
[streams]: http://nodejs.org/api/stream.html
[libpcap]: http://www.tcpdump.org/
[node_pcap]: https://github.com/mranney/node_pcap
[pcap-stream]: https://github.com/wanderview/node-pcap-stream
