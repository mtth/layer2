# Performance benchmarks

Current numbers:

```bash
dot11   1748000 1.2242178135011441 us/p 2139.932738 ms  dat/airtunes-1.pcap.1000
dot11   622000  1.0787237861736334 us/p 670.966195 ms   dat/arp-storm.pcap.1000
dot11   131000  1.2961253893129772 us/p 169.792426 ms   dat/dns-remoteshell.pcap.1000
dot11   780000  1.053788053846154 us/p  821.954682 ms   dat/mesh.pcap.1000
dot11   81000   1.3150146666666667 us/p 106.516188 ms   dat/vnc-sample.pcap.1000
dot11   1093000 1.026839660567246 us/p  1122.335749 ms  dat/wpa-Induction.pcap.1000
pcap    1748000 1.8855998289473683 us/p 3296.028501 ms  dat/airtunes-1.pcap.1000
pcap    622000  1.5693990948553056 us/p 976.166237 ms   dat/arp-storm.pcap.1000
pcap    131000  2.094118412213741 us/p  274.329512 ms   dat/dns-remoteshell.pcap.1000
pcap    780000  1.6928466500000001 us/p 1320.4203 ms    dat/mesh.pcap.1000
pcap    81000   1.8874165555555558 us/p 152.880741 ms   dat/vnc-sample.pcap.1000
pcap    1093000 1.6044667493138152 us/p 1753.682157 ms  dat/wpa-Induction.pcap.1000
```

Captures are downloaded from the [Wireshark sample captures
page](http://wiki.wireshark.org/SampleCaptures) and replicated by a factor of
1000.
