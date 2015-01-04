# Dot11 [![Node Version](https://img.shields.io/node/v/gh-badges.svg?style=flat)](https://www.npmjs.com/package/dot11) [![Build Status](https://travis-ci.org/mtth/dot11.svg?branch=master)](https://travis-ci.org/mtth/dot11)

`dot11` leverages Node.js' built-in [streams][] to provide an intuitive and
efficient interface for frame capture and injection. It also provides parsers
for various link types (e.g. 802.11 with [Radiotap][] headers, raw 802.11,
Ethernet).

```javascript
var dot11 = require('dot11');

var capture = new dot11.capture.Live('en0', {monitor: true});
var decoder = new dot11.transform.Decoder();

capture                     // Stream of buffers (frames' raw bytes).
  .pipe(decoder)            // Stream of objects (decoded frames).
  .on('data', function (frame) {
    console.log(JSON.stringify(frame));
  });
```


## Installation

```bash
$ npm install dot11
```

`dot11` depends on [libpcap][]. Note that you might also need to run live
captures as root.


## Documentation

[API docs][] and several [examples][] are available.


## Benchmarks

Average raw frame throughput (see [Performance benchmarks][benchmarks] for more
details):

+ `dot11`: 0.93 million frames per second
+ [`pcap`][node_pcap]: 0.57 million frames per second
+ [`pcap-stream`][pcap-stream]: 0.21 million frames per second


## Tests

```bash
$ npm test
```

Some tests require an active internet connection.


[Radiotap]: http://www.radiotap.org/
[streams]: http://nodejs.org/api/stream.html
[libpcap]: http://www.tcpdump.org/
[API docs]: https://github.com/mtth/dot11/blob/master/doc/api.md
[examples]: https://github.com/mtth/dot11/blob/master/doc/examples.md
[benchmarks]: https://github.com/mtth/dot11/blob/master/doc/perf.md
[node_pcap]: https://github.com/mranney/node_pcap
[pcap-stream]: https://github.com/wanderview/node-pcap-stream
