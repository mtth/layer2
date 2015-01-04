# Performance benchmarks

Captures are downloaded from the [Wireshark sample captures page][captures].

In order to run these benchmarks you'll need to install the development
dependencies.


## Raw frame throughput

This benchmark measures the rate at which frames are processed while replaying
a capture. This can be viewed as an upper bound on the maximum sustainable live
capture rate.

```bash
$ npm run benchmark-capture

# ...

# dat/airtunes-1.pcap (62543 frames)
77.49 ms (±0.50)         0.81 f/us       645.23 B/us            dot11
117.04 ms (±0.34)        0.53 f/us       427.20 B/us    -34%    pcap
322.25 ms (±2.89)        0.19 f/us       155.16 B/us    -76%    pcap-stream

# dat/arp-storm.pcap (833333 frames)
788.61 ms (±2.12)        1.06 f/us       63.40 B/us             dot11
1379.88 ms (±8.28)       0.60 f/us       36.24 B/us     -43%    pcap
3647.55 ms (±14.98)      0.23 f/us       13.71 B/us     -78%    pcap-stream

# dat/dns-remoteshell.pcap (286218 frames)
302.12 ms (±1.98)        0.95 f/us       165.50 B/us            dot11
484.74 ms (±2.52)        0.59 f/us       103.15 B/us    -38%    pcap
1315.02 ms (±2.65)       0.22 f/us       38.02 B/us     -77%    pcap-stream

# dat/mesh.pcap (328613 frames)
351.54 ms (±2.57)        0.93 f/us       142.23 B/us            dot11
562.23 ms (±2.55)        0.58 f/us       88.93 B/us     -37%    pcap
1517.33 ms (±2.60)       0.22 f/us       32.95 B/us     -77%    pcap-stream

# dat/vnc-sample.pcap (494136 frames)
522.92 ms (±2.69)        0.94 f/us       95.62 B/us             dot11
879.60 ms (±5.13)        0.56 f/us       56.84 B/us     -41%    pcap
2209.95 ms (±5.08)       0.22 f/us       22.62 B/us     -76%    pcap-stream

# dat/wpa-Induction.pcap (337787 frames)
375.63 ms (±5.27)        0.90 f/us       133.11 B/us            dot11
638.59 ms (±11.52)       0.53 f/us       78.30 B/us     -41%    pcap
1609.47 ms (±15.00)      0.21 f/us       31.07 B/us     -77%    pcap-stream
```

Average throughput:

+ `dot11`: 0.93 million frames per second.
+ `pcap`: 0.57 million frames per second.
+ `pcap-stream`: 0.21 million frames per second.

Note that in order to reduce the setup and teardown costs the capture files are
replicated to reach a size of a few megabytes. This way only the marginal frame
processing time matters.

[captures]: http://wiki.wireshark.org/SampleCaptures
