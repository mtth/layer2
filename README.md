Dot11
=====

802.11 packet analyser, inspired by
[node_pcap](https://github.com/mranney/node_pcap).


Installation
------------

```bash
$ npm install dot11
```

`dot11` requires `libpcap`.


Quickstart
----------

```javascript

var dot11 = require('dot11');

// Readable stream from a network interface.
var live = new dot11.capture.Live('en0', {promisc: true});

// Writable stream to the `.pcap` format.
var save = new dot11.capture.Save('log.pcap', live.getDatalink());

// Read 5 seconds' worth of packets from our live stream.
live
  .once('readable', function () {
    setTimeout(function () { live.close(); }, 5000);
  })
  .on('data', function (buf) {
    console.log(buf.length); // Output each packet's length.
  })
  .pipe(save); // Save them to a file to be read later.

```

Api
---

### Capture

#### Class: dot11.capture.Live

##### new dot11.capture.Live(dev, [opts])

+ `dev` String The device to listen to (e.g. `en0`).
+ `options` Object Various options:

  + `monitor` Boolean: Capture in monitor mode. [default: `false`]
  + `promisc` Boolean: Capture in promiscuous mode. [default: `false`]
  + `filter` String: BPF packet filter. [default: `''`]
  + `snapLen` Number: Snapshot length (i.e. how much of each packet is
    retained). [default: `65535`]
  + `bufferSize` Number: Size of temporary buffer used by PCAP to hold packets.
    Larger means more packets can be gathered in fewer dispatch calls (this
    will effectively cap the batchSize option). [default: `1024 * 1024` (1MB)]
  + `batchSize` Number: Number of packets read during each call to PCAP. A
    higher number here is more efficient but runs the risk of overflowing
    internal buffers (especially in the case of replay streams, which can't
    rely on the PCAP dispatch call returning in time). [default: `1000`]

Create a new readable stream from a network interface.

##### live.close()

Terminate the stream.

Note that depending on when this is called, a few more packets might be emitted
before the stream actually closes (this number is guaranteed to be lower than
the capture's batch size).

##### live.isClosed()

+ return Boolean

Check if the capture is still running.

##### live.getStats()

+ return Object

Get PCAP statistics for stream.

This includes total packets received, dropped because of buffer overflow, and
dropped by the interface (e.g. because of filters). See `man pcap_stats` for
more details. Finally, note that these statistics are not exact and can even
mean different things depending on the platform.

##### live.getDatalink()

+ return String

Get output data link type. E.g. `IEEE802_11_RADIO`.

This is useful in particular for creating `Save` streams (see below).

##### live.getSnapLen()

+ return Number

Also useful for saves.


#### Class: dot11.capture.Replay

##### new dot11.capture.Replay(path, [opts])

+ `path` String Path to a saved `.pcap` file.
+ `options` Object Various options:

  + `bufferSize` Number: Size of temporary buffer used by PCAP to hold packets.
    Larger means more packets can be gathered in fewer dispatch calls (this
    will effectively cap the batchSize option). [default: `1024 * 1024` (1MB)]

Readable packet stream from saved file.

Note that unlike the live capture stream, this stream will automatically
close once the end of the file read is reached.

##### live.close()

Terminate the stream.

Note that depending on when this is called, a few more packets might be emitted
before the stream actually closes (this number is guaranteed to be lower than
the capture's batch size).

##### live.isClosed()

+ return Boolean

Check if the capture is still running.

##### live.getDatalink()

+ return String

Get output data link type. E.g. `IEEE802_11_RADIO`.

This is useful in particular for creating `Save` streams (see below).

##### live.getSnapLen()

+ return Number

Also useful for saves.


#### Class: dot11.capture.Save

##### new dot11.capture.Save(path, datalink, [opts])

+ `path` String: The path where the capture will be stored.
+ `datalink` String: The type of link that will be saved.
+ `opts` Object: Optional parameters:

  +`snapLen` Number: The maximum packet capture length to store in the
    file's global header. [default: `65535`]

Writable stream to save packets to the `.pcap` format (compatible with
Wireshark and Tcpdump).

Note that the total length parameter in each packet header will be
assigned to the packet's captured length (and not the original packet
length as it isn't available anymore). This shouldn't be a problem as long
as this class is only used to store the output of a Capture class here
defined (as truncated packets do not get carried over).
