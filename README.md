# Layer2 [![Build Status](https://travis-ci.org/mtth/layer2.svg?branch=master)](https://travis-ci.org/mtth/layer2)

```javascript
var layer2 = require('layer2');

layer2.createInterfaceSniffer('en0', {bufferSize: 1024})
  .on('pdu', function (pdu) {
    // Assuming EthernetII frames.
    var frame = pdu.Ethernet2;
    var src = frame.srcAddr.toString('hex');
    var dst = frame.dstAddr.toString('hex');
    var size = frame.data.length;
    console.log(src + ' -> ' + dst + ' [' + size + ']');
  });
```
