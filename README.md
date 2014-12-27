# Dot11 [![Build Status](https://travis-ci.org/mtth/dot11.svg?branch=master)](https://travis-ci.org/mtth/dot11)

*A powerful yet simple API for Wi-Fi monitoring.*

`dot11` leverages Node.js' [streams](http://nodejs.org/api/stream.html) to
provide an intuitive and efficient interface for packet capture and injection.
It is inspired by other packet capturing libraries (e.g.
[lib_pcap](https://github.com/mranney/node_pcap)).

```javascript
var dot11 = require('dot11');

var capture = new dot11.capture.Live('en0', {promisc: true, monitor: true});
var extractor = new dot11.transform.Extractor();
var decoder = new dot11.transform.Decoder({stringify: true, ignoreErrors: true});

capture                     // Stream of Wi-Fi packets inside Radiotap headers.
  .pipe(extractor)          // Stream of extracted 802.11 frames.
  .pipe(decoder)            // Stream of decoded and stringified 802.11 frames.
  .pipe(process.stdout);    // Output result to console.
```


## Installation

```bash
$ npm install dot11
```

`dot11` depends on [libpcap](http://www.tcpdump.org/) (available by default on
OS X). In order to be able to capture packets, you might also need to run live
captures as root.


## Quickstart

### Capture

In the example below, we create a readable
[`Live`](https://github.com/mtth/dot11/blob/master/doc/api.md#class-dot11capturelive)
stream from the default network interface and store 5 seconds' worth of packets to
a file by writing to a
[`Save`](https://github.com/mtth/dot11/blob/master/doc/api.md#class-dot11capturesave)
stream. We also print each packet's length and the total number of packets
processed at the very end.

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
monitor). A
[`Replay`](https://github.com/mtth/dot11/blob/master/doc/api.md#class-dot11capturereplay)
stream is also available to stream packets from a saved capture file.

### Transform

`dot11` currently has decoders for Radiotap headers and (partially) 802.11
packets. It also features an extractor from Radiotap to the latter. Hopefully
more transformations will be added soon.


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
