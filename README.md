# Layer2 [![Build Status](https://travis-ci.org/mtth/layer2.svg?branch=master)](https://travis-ci.org/mtth/layer2)

```javascript
var layer2 = require('layer2');

layer2.createInterfaceSniffer('en0')
  .on('pdu', function (pdu) {
    // Assuming EthernetII frames.
    var frame = pdu.Ethernet2;
    var src = frame.srcAddr.toString('hex');
    var dst = frame.dstAddr.toString('hex');
    var size = frame.data.length;
    console.log(src + ' -> ' + dst + ' [' + size + ']');
  });
```


## Installation

If you already have `libtins>=3.2` and `avro>=1.7` installed:

```bash
$ npm install layer2
```

Otherwise, you can build both dependencies along with this package by running
(note the last step required to find the libraries at runtime, alternatively
you could install both dependencies globally):

```bash
$ git clone --recursive git@github.com:mtth/layer2.git
$ npm run install-all
$ export LD_LIBRARY_PATH="$(pwd)/etc/deps/libtins/build/lib:$(pwd)/etc/deps/avro/lang/c++/build"
```

In both cases, you will need a compiler with C++11 support.
