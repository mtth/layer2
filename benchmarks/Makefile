.PHONY: all

all: \
	dat/airtunes-1.pcap \
	dat/arp-storm.pcap \
	dat/dns-remoteshell.pcap \
	dat/mesh.pcap \
	dat/vnc-sample.pcap \
	dat/wpa-Induction.pcap

dat/%: | dat
	curl -sSl "http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=$*" >$@

dat:
	mkdir dat
