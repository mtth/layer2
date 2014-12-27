# Dot11 [![Build Status](https://travis-ci.org/mtth/dot11.svg?branch=master)](https://travis-ci.org/mtth/dot11)

*A powerful yet simple API for Wi-Fi monitoring.*

`dot11` leverages Node.js's [streams](http://nodejs.org/api/stream.html) to
provide an intuitive and efficient interface for packet capture and injection.
It is inspired by other packet capturing libraries (e.g.
[lib_pcap](https://github.com/mranney/node_pcap)).


## Installation

```bash
$ npm install dot11
```

`dot11` depends on [libpcap](http://www.tcpdump.org/) (available by default on
OS X). In order to be able to capture packets, you might also need to run live
captures as root.


## Quickstart

In the example below, we create a readable stream from default network
interface and store 5 seconds' worth of packets to `log.pcap`. We also print
each packet's length and the total number of packets processed.

```javascript
var dot11 = require('dot11');

var liveStream = new dot11.capture.Live(); // Readable stream of packets.
var saveStream = new dot11.capture.Save('log.pcap'); // Writable stream.

liveStream
  .close(5000)
  .on('data', function (buf) { console.log('Read packet of length: ' + buf.length); })
  .on('end', function () { console.log('Read ' + this.getStats().psRecv + ' packets!'); })
  .pipe(saveStream);
```

The above example is only using the default options, it is possible to specify
which interface to listen to and what modes to activate (e.g. promiscuous or
monitor). A `Replay` stream is also available to stream packets from a saved
capture file.


## Documentation

You can find the API docs
[here](https://github.com/mtth/dot11/blob/master/doc/api.md).


## Benchmarks

Average raw packet throughput (see [Performance
benchmarks](https://github.com/mtth/dot11/blob/master/doc/perf.md) for more
details):

+ `pcap`: 0.56 million packets per second.
+ `dot11`: 0.86 million packets per second.


## Tests

```bash
$ npm test
```

Some tests require an active internet connection.
