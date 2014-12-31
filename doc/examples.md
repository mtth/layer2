# Examples


All the examples below assume that the capture interface is `en0` and the
corresponding link type is `IEEE802_11_RADIO` (i.e. 802.11 frames with a
Radiotap header). This is the default on OS X in monitor mode.


## 802.11 headers

```javascript
var capture = new dot11.capture.Live('en0', {monitor: true});
var extractor = new dot11.transform.Extractor();
var decoder = new dot11.transform.Decoder({stringify: true});

capture
  .pipe(extractor)
  .pipe(decoder)
  .pipe(process.stdout);
```

This output can be directly piped to [jq](http://stedolan.github.io/jq/) for
easy parsing.


## 802.11 overall activity

Also output total frames and fraction of invalid frames at the end. Note that
this measures total number of frames captured, not the total data size (in
particular, this includes all control and management frames).

```javascript
var capture = new dot11.capture.Live('en0', {monitor: true});
var extractor = new dot11.transform.Extractor();
var decoder = new dot11.transform.Decoder();

var nValid = 0;
var nInvalid = 0;

capture
  .pipe(extractor)
  .pipe(decoder)
  .on('data', function () {
    nValid++;
    process.stderr.write('.');
  })
  .on('invalid', function () {
    nInvalid++;
    process.stderr.write('I');
  });

process.on('SIGINT', function () {
  capture
    .on('close', function () {
      console.log(util.format(
        '\n\nTotal: %s (%s%% invalid).',
        (nValid + nInvalid), Math.round(100 * nInvalid / (nValid + nInvalid))
      ));
    })
    .close();
});
```

Be sure to check the
[FAQ](https://github.com/mtth/dot11/blob/master/doc/faq.md#im-seeing-a-lot-of-invalid-frames-why-is-this)
if you are seeing a lot of invalid frames.


## 802.11 data traffic

Outputs the total number of data bytes received per MAC address in the last 10
seconds.

```javascript
var capture = new dot11.capture.Live('en0', {monitor: true});
var extractor = new dot11.transform.Extractor();
var decoder = new dot11.transform.Decoder();
var nBytesReceived = {};

capture
  .close(10000)
  .pipe(extractor)
  .pipe(decoder)
  .on('data', function (frame) {
    if (frame.type === 'data' && frame.body) {
      var sa = frame.sa;
      nBytesReceived[sa] = (nBytesReceived[sa] || 0) + frame.body.length;
    }
  })
  .on('end', function () {
    console.dir(nBytesReceived);
  });
```


## MAC addresses of nearby access points

This script will run for 10 seconds and print the list of discovered BSSIDs
(i.e. typically MAC addresses) along with the associated number of frames
captured.

```javascript
var capture = new dot11.capture.Live('en0', {monitor: true});
var extractor = new dot11.transform.Extractor();
var decoder = new dot11.transform.Decoder();
var bssids = {};

capture
  .close(10000)
  .pipe(extractor)
  .pipe(decoder)
  .on('data', function (frame) {
    var bssid = frame.bssid;
    if (bssid) {
      bssids[bssid] = (bssids[bssid] || 0) + 1;
    }
  })
  .on('end', function () {
    console.dir(bssids);
  });
```
