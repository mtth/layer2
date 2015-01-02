# API


## Capture

This module contains the following 3 streams:

+ [`Live`](https://github.com/mtth/dot11/blob/master/doc/api.md#class-dot11capturelive)
  A duplex stream of frames from a live network interface.
+ [`Replay`](https://github.com/mtth/dot11/blob/master/doc/api.md#class-dot11capturereplay)
  A readable stream of frames from a saved capture file.
+ [`Save`](https://github.com/mtth/dot11/blob/master/doc/api.md#class-dot11capturesave)
  A writable stream of frames to write to a capture file.


### Class: dot11.capture.Live

The `Live` class is a duplex (i.e. readable and writable) stream of frames. An
instance of this class will listen to a given network interface (e.g. `'en0'`)
and stream out any frames available. Any frames written to the stream will be
injected via this same interface (if supported by your OS).

```javascript
var nFrames = 0;

// Counting the number of frames received in 10 seconds.
new Live('en0', {promisc: true, monitor: true})
  .on('data', function (buf) { nFrames++; })
  .on('end', function () { console.log('Received ' + nFrames + ' frames.'); })
  .close(10000);
```

#### Event: 'readable'

Emitted when the first frame can be read.

#### Event: 'data'

+ `frame` {Buffer} A frame.

Emitted each time a frame is available for reading.

Attaching a handler to this event (and `pipe`ing) is the preferred way of
reading this stream of frames.

#### Event: 'end'

Emitted after `close` is called but before the underlying device is closed.

#### Event 'finish'

Emitted right after the writable end of the stream closes (e.g. caused by
calling the `end` method). Similarly to the `'end'` event, the underlying
resource is guaranteed to still be open at this time.

#### Event 'error'

+ `err` {Error} The error that caused this.

Emitted when something went wrong. Currently the main cause is frame
truncation (if the `maxFrameSize` parameter is too small).

#### Event: 'fetch'

Emitted each time a batch of frames is fetched from the underlying resource.

+ `batchUsage` {Number} Fraction of batch size used. This should be kept
  consistently under 1 in order to avoid filling up PCAP's buffer and losing
  frames there.
+ `bufferUsage` {Number} Fraction of the buffer used. This is only minimally
  useful for live streams, as we are guaranteed that the internal buffer will
  not overflow (the size is chosen appropriately w.r.t the underlying PCAP
  buffer).

#### Event: 'close'

Emitted after the underlying device is closed.

Most methods on the stream will be unavailable at this point.

This event is guaranteed to be emitted after both `'end'` and `'finish'`
events. This is useful as it lets us use methods such as `getStats` from these
events' handlers.

#### new dot11.capture.Live([dev], [opts])

Create a new readable stream from a network interface.

+ `dev` {String} The device to listen to (e.g. `'en0'`). If `dev` is `null` or
  `undefined`, the default network interface will be used. On some platforms,
  `'any'` can be specified to listen on all interfaces.
+ `opts` {Object} Various options:
  + `monitor` {Boolean} Capture in monitor mode. [default: `false`]
  + `promisc` {Boolean} Capture in promiscuous mode. [default: `false`]
  + `maxFrameSize` {Number} Frame snapshot length (i.e. how much of each
    frame is retained). [default: `65535`]
  + `bufferSize` {Number} Size of temporary buffer used by PCAP to hold frames.
    Larger means more frames can be gathered in fewer dispatch calls (this
    will effectively cap the batchSize option). [default: `1024 * 1024`]
  + `batchSize` {Number} Number of frames read during each call to PCAP. A
    higher number here is more efficient but runs the risk of overflowing
    internal buffers (especially in the case of replay streams, which can't
    rely on the PCAP dispatch call returning in time). [default: `1000`]

Note that a section of length `maxFrameSize` is reserved at the end of the
temporary buffer to protect from overflows. This way, the internal mechanism
that fetches frames by batch (for performance), has enough time to stop
without losing any frames.

#### live.read()

+ return {Buffer}

Get a frame from the stream if available, and `null` otherwise.

To guarantee that some data is available, this should only be called after the `'readable'` event has been triggered:

```javascript
var frames = [];
live.on('readable', function () {
    var buf;
    while ((buf = this.read()) !== null) {
      frames.push(buf);
    }
  });
```

#### live.close([timeout])

Terminate the stream and close underlying resources.

+ `timeout` {Number} Timeout in milliseconds after which to close the stream.
  [default: `0`]

Note that depending on when this is called, a few more frames might be emitted
before the stream actually closes (this number is guaranteed to be lower than
the capture's batch size).

#### live.getStats()

Get PCAP statistics for stream.

+ return {Object}

Sample result: `{ psRecv: 87, psDrop: 0, psIfDrop: 0 }`.

This includes total frames received, dropped because of buffer overflow, and
dropped by the interface (e.g. because of filters). See `man pcap_stats` for
more details. Finally, note that these statistics are not exact and can even
mean different things depending on the platform.

#### live.getLinkType()

Get output data link type. E.g. `'IEEE802_11_RADIO'`.

+ return {String}

See [here](http://www.tcpdump.org/linktypes.html) for the full list. This is
useful in particular for creating `Save` streams (see below).

#### live.getMaxFrameSize()

Get underlying snapshot length.

+ return {Number}

Also useful for creating saves.


### Class: dot11.capture.Replay

Readable frame stream from a saved file.

#### Event: 'readable'

Emitted when the first frame can be read.

#### Event: 'data'

+ `frame` {Buffer} A frame.

Emitted each time a frame is available for reading.

#### Event: 'end'

Emitted when the end of the file is reached.

Similarly to `Live`, this will be emitted before the underlying device is
closed.

#### Event 'error'

+ `err` {Error} The error that caused this.

Emitted when something went wrong. Currently the main cause is frame
truncation (if the `maxFrameSize` parameter is too small).

#### Event: 'fetch'

Emitted each time a batch of frames is fetched from the underlying resource.

+ `batchUsage` {Number} Fraction of batch size used. In the case of a `Replay`,
  this fraction will typically be very close to `1`.
+ `bufferUsage` {Number} Fraction of the buffer used. This is useful to avoid
  having too many dispatch loops interrupted prematurely (see `'break'` event
  below).

#### Event 'break'

If the temporary buffer size is too small to hold a full batch of frames, the
frame dispatching loop must be interrupted. When this happens, this event is
emitted. As good performance relies on minimizing the total number of
dispatches (i.e. fetches), tracking this event can be useful when slowness
occurs (or simply in order to optimize the batch and buffer size).

#### Event: 'close'

Emitted when the underlying device is closed.

This event is guaranteed to be emitted after the `'end'` event.

#### new dot11.capture.Replay(path, [opts])

+ `path` {String} Path to a saved capture file (PCAP format).
+ `opts` {Object} Various options:
  + `bufferSize` {Number} Size of temporary buffer used by PCAP to hold frames.
    Larger means more frames can be gathered in fewer dispatch calls (this
    will effectively cap the batchSize option). [default: `1024 * 1024` (1MB)]
  + `batchSize` {Number} Number of frames read during each call to PCAP. A
    higher number here is more efficient but runs the risk of overflowing
    internal buffers (especially in the case of replay streams, which can't
    rely on the PCAP dispatch call returning in time). [default: `1000`]

Note that unlike the live capture stream, this stream will automatically
close once the end of the file read is reached.

#### replay.close([timeout])

Terminate the stream.

+ `timeout` {Number} Timeout in milliseconds after which to close the stream.
  [default: `0`]

Note that even if timeout is set to 0, a few more frames might be emitted
before the stream actually closes.

#### replay.getLinkType()

Get output data link type. E.g. `'IEEE802_11_RADIO'`.

+ return {String}

This is useful in particular for creating `Save` streams (see below).

#### replay.getMaxFrameSize()

Also useful for saves.

+ return {Number}


### Class: dot11.capture.Save

A writable stream useful to store captures for later. The format used is
compatible with Wireshark and Tcpdump and can be read normally by them.


#### Event 'finish'

Emitted right after the stream ends (e.g. caused by calling the `end` method).
The underlying resource is guaranteed to still be open at this time.

#### Event 'error'

+ `err` {Error} The error that caused this.

Emitted when something went wrong.

#### Event: 'close'

Emitted after the underlying device is closed.

Most methods on the stream will be unavailable at this point.

#### new dot11.capture.Save(path, [opts])

Writable stream to save frames to the `.pcap` format (compatible with
Wireshark and Tcpdump).

+ `path` {String} The path where the capture will be stored.
+ `opts` {Object} Optional parameters:
  + `linkType` {String} The data link type. If not specified here, it will be
    inferred from the first stream piped.
  + `maxFrameSize` {Number} The maximum frame capture length (to store in the
    file's global header). [default: `65535`]

Note that the total length parameter in each frame header will be
assigned to the frame's captured length (and not the original frame
length as it isn't available anymore). This shouldn't be a problem as long
as this class is only used to store the output of a Capture class here
defined (as truncated frames do not get carried over).


## Decode

### Class: dot11.transform.Decoder([opts])

A duplex stream used to transform raw frames (i.e. buffers) to parsed objects.

`dot11` currently supports decoding the following link types:

+ `IEEE802_11_RADIO` (Radiotap).
+ `IEEE802_11` (raw 802.11).
+ `EN10MB` (Ethernet).


#### Event: 'readable'

Emitted when the first frame can be read.

#### Event: 'data'

+ `frame` {Object | String} A (potentially stringified) object representation
  of the frame.

Emitted each time a frame is available for reading.

#### Event 'invalid'

+ `data` {Buffer} The invalid frame.
+ `err` {Error} Description of why this frame is invalid.

Emitted when a frame has an invalid checksum.

We are not emitting errors here because node would unpipe any readable stream
writing to this decoder when it emits an error (and it is expensive to reattach
them each time). Moreover these are hard to control since there are corrupted
frames relatively often.

#### Event: 'end'

Emitted when there are no more frames to read.

#### Event 'error'

+ `err` {Error} The error that caused this.

Emitted when something fatal happened (e.g. invalid link type).

#### new dot11.transform.Decoder([opts])

+ `opts` {Object} Various options:
  + `linkType` {String} The data link type to be decoded. If the stream is
    piped to from another `dot11` stream, this will be inferred automatically.
  + `stringify` {Boolean} Jsonify the decoded objects (useful for example if
    piping to `process.stdout`). [default: `false`]

#### getLinkType()

Get decoded data link type.
