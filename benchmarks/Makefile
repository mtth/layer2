.PHONY: all

all: \
	node_modules \
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

node_modules:
	npm install --prefix . pcap pcap-stream tmp
	find . -name Debug -type d -execdir mv {} Release \; -prune
	# Rename build directories (in case node was compiled with debug flags).
