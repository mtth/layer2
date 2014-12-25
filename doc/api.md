# API


## Capture

This module contains different packet streams.


### Class: dot11.capture.Live

#### new dot11.capture.Live(dev, [opts])

Create a new readable stream from a network interface.

+ `dev` {String} The device to listen to (e.g. `'en0'`).
+ `opts` {Object} Various options:
  + `monitor` {Boolean} Capture in monitor mode. [default: `false`]
  + `promisc` {Boolean} Capture in promiscuous mode. [default: `false`]
  + `filter` {String} BPF packet filter. [default: `''`]
  + `snapLen` {Number} Snapshot length (i.e. how much of each packet is
    retained). [default: `65535`]
  + `bufferSize` {Number} Size of temporary buffer used by PCAP to hold packets.
    Larger means more packets can be gathered in fewer dispatch calls (this
    will effectively cap the batchSize option). [default: `1024 * 1024`]
  + `batchSize` {Number} Number of packets read during each call to PCAP. A
    higher number here is more efficient but runs the risk of overflowing
    internal buffers (especially in the case of replay streams, which can't
    rely on the PCAP dispatch call returning in time). [default: `1000`]

#### live.close()

Terminate the stream.

Note that depending on when this is called, a few more packets might be emitted
before the stream actually closes (this number is guaranteed to be lower than
the capture's batch size).

#### live.isClosed()

Check if the capture is still running.

+ return {Boolean}

#### live.getStats()

Get PCAP statistics for stream.

+ return {Object}

This includes total packets received, dropped because of buffer overflow, and
dropped by the interface (e.g. because of filters). See `man pcap_stats` for
more details. Finally, note that these statistics are not exact and can even
mean different things depending on the platform.

#### live.getDatalink()

Get output data link type. E.g. `'IEEE802_11_RADIO'`.

+ return {String}

This is useful in particular for creating `Save` streams (see below).

#### live.getSnapLen()

Get underlying snapshot length.

+ return {Number}

Also useful for saves.


### Class: dot11.capture.Replay

#### new dot11.capture.Replay(path, [opts])

Readable packet stream from saved file.

+ `path` {String} Path to a saved capture file (pcap format).
+ `opts` {Object} Various options:
  + `bufferSize` {Number} Size of temporary buffer used by PCAP to hold packets.
    Larger means more packets can be gathered in fewer dispatch calls (this
    will effectively cap the batchSize option). [default: `1024 * 1024` (1MB)]

Note that unlike the live capture stream, this stream will automatically
close once the end of the file read is reached.

#### replay.close()

Terminate the stream.

Note that depending on when this is called, a few more packets might be emitted
before the stream actually closes (this number is guaranteed to be lower than
the capture's batch size).

#### replay.isClosed()

Check if the capture is still running.

+ return {Boolean}

#### replay.getDatalink()

Get output data link type. E.g. `'IEEE802_11_RADIO'`.

+ return {String}

This is useful in particular for creating `Save` streams (see below).

#### replay.getSnapLen()

Also useful for saves.

+ return {Number}


### Class: dot11.capture.Save

#### new dot11.capture.Save(path, datalink, [opts])

Writable stream to save packets to the `.pcap` format (compatible with
Wireshark and Tcpdump).

+ `path` {String} The path where the capture will be stored.
+ `datalink` {String} The type of link that will be saved.
+ `opts` {Object} Optional parameters:
  + `snapLen` {Number} The maximum packet capture length to store in the
    file's global header. [default: `65535`]

Note that the total length parameter in each packet header will be
assigned to the packet's captured length (and not the original packet
length as it isn't available anymore). This shouldn't be a problem as long
as this class is only used to store the output of a Capture class here
defined (as truncated packets do not get carried over).


## Transform

Various decoders and extractors (soon).
