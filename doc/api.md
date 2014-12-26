# API


## Capture

This module contains different packet streams.


### Class: dot11.capture.Live

On top of the usual stream events, the following are available:

#### Event: 'fetch'

Emitted each time a batch of packets is fetched from the underlying resource.

+ `batchUsage` {Number} Fraction of batch size used. In live mode, this should
  be consistently under 1 in order to avoid filling up PCAP's buffer and losing
  packets there.
+ `bufferUsage` {Number} Fraction of the buffer used. This is mostly useful in
  replay mode, as in live mode we are guaranteed (if the buffer sizes between
  here and PCAP as equal) that this will never overflow.

#### Event: 'close'

Emitted when the underlying device is closed.

This event is guaranteed to be emitted after both `'end'` and `'finish'`
events. This is useful as it lets us use methods such as `getStats` from these
events' handlers.

#### new dot11.capture.Live(dev, [opts])

Create a new readable stream from a network interface.

+ `dev` {String} The device to listen to (e.g. `'en0'`).
+ `opts` {Object} Various options:
  + `monitor` {Boolean} Capture in monitor mode. [default: `false`]
  + `promisc` {Boolean} Capture in promiscuous mode. [default: `false`]
  + `filter` {String} BPF packet filter. [default: `''`]
  + `maxPacketSize` {Number} Packet snapshot length (i.e. how much of each
    packet is retained). [default: `65535`]
  + `bufferSize` {Number} Size of temporary buffer used by PCAP to hold packets.
    Larger means more packets can be gathered in fewer dispatch calls (this
    will effectively cap the batchSize option). [default: `1024 * 1024`]
  + `batchSize` {Number} Number of packets read during each call to PCAP. A
    higher number here is more efficient but runs the risk of overflowing
    internal buffers (especially in the case of replay streams, which can't
    rely on the PCAP dispatch call returning in time). [default: `1000`]

Note that a section of length `maxPacketSize` is reserved at the end of the
temporary buffer to protect from overflows. This way, the internal mechanism
that fetches packets by batch (for performance), has enough time to stop
without losing any packets.

#### live.close([timeout])

Terminate the stream and close underlying resources.

+ `timeout` {Number} Timeout in milliseconds after which to close the stream.
  [default: `0`]

Note that depending on when this is called, a few more packets might be emitted
before the stream actually closes (this number is guaranteed to be lower than
the capture's batch size).

#### live.getStats()

Get PCAP statistics for stream.

+ return {Object}

Sample result:

```javascript
{
  psRecv: 87,
  psDrop: 0,
  psIfDrop: 0
}
```

This includes total packets received, dropped because of buffer overflow, and
dropped by the interface (e.g. because of filters). See `man pcap_stats` for
more details. Finally, note that these statistics are not exact and can even
mean different things depending on the platform.

#### live.getLinkType()

Get output data link type. E.g. `'IEEE802_11_RADIO'`.

+ return {String}

See [here](http://www.tcpdump.org/linktypes.html) for the full list. This is
useful in particular for creating `Save` streams (see below).

#### live.getMaxPacketSize()

Get underlying snapshot length.

+ return {Number}

Also useful for saves.


### Class: dot11.capture.Replay

Readable packet stream from a saved file.

#### Event: 'fetch'

Emitted each time a batch of packets is fetched from the underlying resource.

+ `batchUsage` {Number} Fraction of batch size used. In live mode, this should
  be consistently under 1 in order to avoid filling up PCAP's buffer and losing
  packets there.
+ `bufferUsage` {Number} Fraction of the buffer used. This is mostly useful in
  replay mode, as in live mode we are guaranteed (if the buffer sizes between
  here and PCAP as equal) that this will never overflow.

#### Event: 'close'

Emitted when the underlying device is closed.

This event is guaranteed to be emitted after the `'end'` event.

#### new dot11.capture.Replay(path, [opts])

+ `path` {String} Path to a saved capture file (PCAP format).
+ `opts` {Object} Various options:
  + `bufferSize` {Number} Size of temporary buffer used by PCAP to hold packets.
    Larger means more packets can be gathered in fewer dispatch calls (this
    will effectively cap the batchSize option). [default: `1024 * 1024` (1MB)]

Note that unlike the live capture stream, this stream will automatically
close once the end of the file read is reached.

#### replay.close([timeout])

Terminate the stream.

+ `timeout` {Number} Timeout in milliseconds after which to close the stream.
  [default: `0`]

Note that even if timeout is set to 0, a few more packets might be emitted
before the stream actually closes.

#### replay.getLinkType()

Get output data link type. E.g. `'IEEE802_11_RADIO'`.

+ return {String}

This is useful in particular for creating `Save` streams (see below).

#### replay.getMaxPacketSize()

Also useful for saves.

+ return {Number}


### Class: dot11.capture.Save

#### new dot11.capture.Save(path, [opts])

Writable stream to save packets to the `.pcap` format (compatible with
Wireshark and Tcpdump).

+ `path` {String} The path where the capture will be stored.
+ `opts` {Object} Optional parameters:
  + `linkType` {String} The data link type. If not specified here, it will be
    inferred from the first stream piped.
  + `maxPacketSize` {Number} The maximum packet capture length to store in the
    file's global header. [default: `65535`]

Note that the total length parameter in each packet header will be
assigned to the packet's captured length (and not the original packet
length as it isn't available anymore). This shouldn't be a problem as long
as this class is only used to store the output of a Capture class here
defined (as truncated packets do not get carried over).


## Transform

Various decoders and extractors (soon).
