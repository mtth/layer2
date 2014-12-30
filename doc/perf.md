# Performance benchmarks


Captures are downloaded from the [Wireshark sample captures
page](http://wiki.wireshark.org/SampleCaptures) and replicated by a factor of
1000.


## Raw frame throughput

To run this benchmark:

```bash
$ npm run perf-capture
```

Current numbers for pass-through implementation.

Columns:

+ Package
+ Total frames processed
+ Total time (milliseconds)
+ Average time per frame (nanoseconds)
+ Capture file

```bash
dot11           1748000 frames  2201 ms   1259 ns/frame   dat/airtunes-1.pcap.1000
dot11           622000 frames   638 ms    1026 ns/frame   dat/arp-storm.pcap.1000
dot11           131000 frames   190 ms    1456 ns/frame   dat/dns-remoteshell.pcap.1000
dot11           780000 frames   847 ms    1087 ns/frame   dat/mesh.pcap.1000
dot11           81000 frames    106 ms    1312 ns/frame   dat/vnc-sample.pcap.1000
dot11           1093000 frames  1122 ms   1026 ns/frame   dat/wpa-Induction.pcap.1000
pcap            1748000 frames  3451 ms   1974 ns/frame   dat/airtunes-1.pcap.1000
pcap            622000 frames   1027 ms   1651 ns/frame   dat/arp-storm.pcap.1000
pcap            131000 frames   251 ms    1919 ns/frame   dat/dns-remoteshell.pcap.1000
pcap            780000 frames   1263 ms   1619 ns/frame   dat/mesh.pcap.1000
pcap            81000 frames    153 ms    1888 ns/frame   dat/vnc-sample.pcap.1000
pcap            1093000 frames  1792 ms   1639 ns/frame   dat/wpa-Induction.pcap.1000
pcap-stream     1748000 frames  8306 ms   4751 ns/frame   dat/airtunes-1.pcap.1000
pcap-stream     622000 frames   2689 ms   4323 ns/frame   dat/arp-storm.pcap.1000
pcap-stream     131000 frames   601 ms    4587 ns/frame   dat/dns-remoteshell.pcap.1000
pcap-stream     780000 frames   3802 ms   4875 ns/frame   dat/mesh.pcap.1000
pcap-stream     81000 frames    398 ms    4924 ns/frame   dat/vnc-sample.pcap.1000
pcap-stream     1093000 frames  4941 ms   4521 ns/frame   dat/wpa-Induction.pcap.1000
```

Average throughput:

+ `dot11`: 0.86 million frames per second
+ [`pcap`](https://github.com/mranney/node_pcap): 0.56 million frames per second
+ [`pcap-stream`](https://github.com/wanderview/node-pcap-stream): 0.21 million frames per second
