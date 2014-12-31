# Examples


All the examples below assume that the capture interface is `en0` and the
corresponding link type is `IEEE802_11_RADIO` (i.e. 802.11 frames with a
Radiotap header). This is the default on OS X in monitor mode.


## 802.11 frame headers

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


## Overall 802.11 activity

Also output total frames and fraction of invalid frames at the end.

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
