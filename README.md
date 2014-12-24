Dot11
=====

802.11 packet analyser, inspired by
[node_pcap](https://github.com/mranney/node_pcap).


Quickstart
----------

```javascript

var dot11 = require('dot11');

// Replay stream from saved capture file.
var maxLength = 0;
new dot11.stream.Replay('log.pcap')
  .on('data', function (data) { // data is a buffer containing the raw packet.
    maxLength = Math.max(data.length, maxLength);
  })
  .on('end', function () {
    console.log('Longest length: ' + maxLength);
  });

// Live stream from en0 interface.
new dot11.stream.Live('en0')
  .on('data', function (data) {
    console.log(data.length);
  });

```
