Dot11
=====

Wi-Fi packets streams!


Motivation
----------

`dot11` aims to provide an efficient and powerful yet simple interface to Wi-Fi
(802.11) transmissions, both for capturing and emitting. If you know how to use
streams, you know how to use `dot11` (and event if not, it's not hard)!

Its API is similar to the built-in `Socket`, but for lower network layers. It
is originally inspired by [node_pcap](https://github.com/mranney/node_pcap).


Installation
------------

```bash
$ npm install dot11
```

`dot11` depends on [libpcap](http://www.tcpdump.org/) (available by default on
OS X). In order to be able to capture packets, you might also need to run live
captures as root.


Quickstart
----------

In the example below, we create a readable stream from the `en0` network
interface in promiscuous mode and store 5 seconds' worth of packets. We also
print each packet's length and the total number of packets processed.

```javascript
var dot11 = require('dot11');

new dot11.capture.Live('en0', {promisc: true})
  .close(5000)
  .on('data', function (buf) {
    console.log('Read packet of length: ' + buf.length);
  })
  .on('end', function () {
    console.log('Read ' + this.getStats().psRecv + ' packets!');
  })
  .pipe(new dot11.capture.Save('log.pcap'));
```

A `Replay` stream is also available to stream packets from a saved capture
file.


Documentation
-------------

You can find the API docs
[here](https://github.com/mtth/dot11/blob/master/doc/api.md).


Tests
-----

```bash
$ npm test
```

Some tests currently require an active internet connection.
