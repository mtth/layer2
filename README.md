# Layer2 [![Build Status](https://travis-ci.org/mtth/layer2.svg?branch=master)](https://travis-ci.org/mtth/layer2)

`layer2` leverages Node.js' built-in [streams][] to provide an intuitive and
efficient interface to the [data link layer][wiki].

```javascript
var layer2 = require('layer2');

new layer2.capture.Live('en0', {monitor: true})
  .on('data', function (frame) {
    var pdu = frame.getPdu(layer2.pduTypes.DOT11_BEACON);
    if (pdu) {
      console.log(pdu.getCapabilities());
    }
  });
```


## Installation

```bash
$ npm install layer2
```

`layer2` leverages [libpcap][] to capture frames and [libtins][] to decode
them.


## Documentation

[API docs](doc/api.md) and several [examples](examples/) are available.


## Tests

```bash
$ npm test
```

Some tests require an active internet connection.


[streams]: http://nodejs.org/api/stream.html
[wiki]: http://en.wikipedia.org/wiki/Data_link_layer
[libpcap]: http://www.tcpdump.org/
[libtins]: http://libtins.github.io/
