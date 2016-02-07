# Layer2 [![Build Status](https://travis-ci.org/mtth/layer2.svg?branch=master)](https://travis-ci.org/mtth/layer2)

```javascript
var layer2 = require('layer2');

layer2.createInterfaceSniffer('eth0')
  .on('pdu', function (pdu) {
    var frame = pdu.frame.Ethernet2;
    var src = frame.srcAddr;
    var dst = frame.dstAddr;
    console.log(src + ' -> ' + dst + ' [' + pdu.size + ']');
  });
```


## Installation

You will need a compiler with C++11 support, then if you already have
`avro>=1.7` and `tins>=3.2` installed:

```bash
$ npm install layer2
```

Otherwise, you can build both dependencies along with this package by running:

```bash
$ git clone --recursive git@github.com:mtth/layer2.git
$ cd layer2
$ npm run install-all
$ export LD_LIBRARY_PATH="$(pwd)/etc/deps/libtins/build/lib:$(pwd)/etc/deps/avro/lang/c++/build:$LD_LIBRARY_PATH"
```

Note the last step required to find the libraries at runtime (alternatively you
could install both dependencies globally).


## Documentation

+ [API](https://github.com/mtth/layer2/wiki/API)
