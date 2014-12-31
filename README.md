# Dot11 [![Build Status](https://travis-ci.org/mtth/dot11.svg?branch=master)](https://travis-ci.org/mtth/dot11)

*A streaming API for Wi-Fi traffic monitoring.*

`dot11` leverages Node.js' built-in
[streams](http://nodejs.org/api/stream.html) to provide an intuitive and
efficient interface for frame capture and injection. It also provides parsers
for various headers (e.g. 802.11 and Ethernet).

```javascript
var dot11 = require('dot11');

var capture = new dot11.capture.Live('en0', {monitor: true});
var extractor = new dot11.transform.Extractor();
var decoder = new dot11.transform.Decoder({stringify: true});

capture                     // 802.11 frames with Radiotap headers.
  .pipe(extractor)          // 802.11 frames.
  .pipe(decoder)            // Decoded and stringified 802.11 frames.
  .pipe(process.stdout);
```


## Installation

```bash
$ npm install dot11
```

`dot11` depends on [libpcap](http://www.tcpdump.org/) (available by default on
OS X). Note that you might also need to run live captures as root.

It also features an extractor from Radiotap to 802.11 frames.

Hopefully more transformations will be added soon.


## Documentation

You can find the API docs
[here](https://github.com/mtth/dot11/blob/master/doc/api.md). Several
[examples](https://github.com/mtth/dot11/blob/master/doc/examples.md) are also
available.


## Benchmarks

Average raw frame throughput (see [Performance
benchmarks](https://github.com/mtth/dot11/blob/master/doc/perf.md) for more
details):

+ `dot11`: 0.86 million frames per second
+ [`pcap`](https://github.com/mranney/node_pcap): 0.56 million frames per second
+ [`pcap-stream`](https://github.com/wanderview/node-pcap-stream): 0.21 million frames per second


## Tests

```bash
$ npm test
```

Some tests require an active internet connection.
