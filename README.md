# Layer2 [![Build Status](https://travis-ci.org/mtth/layer2.svg?branch=master)](https://travis-ci.org/mtth/layer2)

```javascript
var layer2 = require('layer2');

layer2.createInterfaceSniffer('en0')
  .on('pdu', function (pdu) {
    // Assuming EthernetII frames.
    var src = pdu.srcAddr;
    var dst = pdu.dstAddr;
    console.log('captured frame ' + src + ' -> ' + dst);
  });
```
