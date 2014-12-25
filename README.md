Dot11
=====

802.11 packet analyser, inspired by
[node_pcap](https://github.com/mranney/node_pcap).


Quickstart
----------

```javascript

var dot11 = require('dot11');

// Readable stream from a network interface.
var live = new dot11.capture.Live('en0', {promisc: true});

// Writable stream to the `.pcap` format.
var save = new dot11.capture.Save('log.pcap', live.getDatalink());

// Read 5 seconds' worth of packets from our live stream.
live
  .once('readable', function () {
    setTimeout(function () { live.close(); }, 5000);
  })
  .on('data', function (buf) {
    console.log(buf.length); // Output each packet's length.
  })
  .pipe(save); // Save them to a file to be read later.

```
