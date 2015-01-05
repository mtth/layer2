# Performance benchmarks

Captures are downloaded from the [Wireshark sample captures page][captures].

In order to run these benchmarks you'll need to install the development
dependencies.


## Capture

This benchmark measures the rate at which frames are processed while replaying
a capture. This can be viewed as an upper bound on the maximum sustainable live
capture rate.

```bash
$ npm run benchmark-capture

# ...

# airtunes-1 (62543 frames, 20 runs)
77.49 ms (±0.50)         0.81 f/us       645.23 B/us            layer2
117.04 ms (±0.34)        0.53 f/us       427.20 B/us    -34%    pcap
322.25 ms (±2.89)        0.19 f/us       155.16 B/us    -76%    pcap-stream

# arp-storm (833333 frames, 20 runs)
788.61 ms (±2.12)        1.06 f/us       63.40 B/us             layer2
1379.88 ms (±8.28)       0.60 f/us       36.24 B/us     -43%    pcap
3647.55 ms (±14.98)      0.23 f/us       13.71 B/us     -78%    pcap-stream

# dns-remoteshell (286218 frames, 20 runs)
302.12 ms (±1.98)        0.95 f/us       165.50 B/us            layer2
484.74 ms (±2.52)        0.59 f/us       103.15 B/us    -38%    pcap
1315.02 ms (±2.65)       0.22 f/us       38.02 B/us     -77%    pcap-stream

# mesh (328613 frames, 20 runs)
351.54 ms (±2.57)        0.93 f/us       142.23 B/us            layer2
562.23 ms (±2.55)        0.58 f/us       88.93 B/us     -37%    pcap
1517.33 ms (±2.60)       0.22 f/us       32.95 B/us     -77%    pcap-stream

# vnc-sample (494136 frames, 20 runs)
522.92 ms (±2.69)        0.94 f/us       95.62 B/us             layer2
879.60 ms (±5.13)        0.56 f/us       56.84 B/us     -41%    pcap
2209.95 ms (±5.08)       0.22 f/us       22.62 B/us     -76%    pcap-stream

# wpa-Induction (337787 frames, 20 runs)
375.63 ms (±5.27)        0.90 f/us       133.11 B/us            layer2
638.59 ms (±11.52)       0.53 f/us       78.30 B/us     -41%    pcap
1609.47 ms (±15.00)      0.21 f/us       31.07 B/us     -77%    pcap-stream
```

![Capture benchmark](img/capture.png)

Average raw throughput:

+ `layer2`: 0.93 million frames per second.
+ [`pcap`][node_pcap]: 0.57 million frames per second.
+ [`pcap-stream`][pcap-stream]: 0.21 million frames per second.

Note that in order to reduce the setup and teardown costs the capture files are
replicated to reach a size of a few megabytes. This way only the marginal frame
processing time matters.

[captures]: http://wiki.wireshark.org/SampleCaptures
[node_pcap]: https://github.com/mranney/node_pcap
[pcap-stream]: https://github.com/wanderview/node-pcap-stream
