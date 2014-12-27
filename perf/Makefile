.PHONY: all

all: dot11 pcap

%: | dat
	@for name in dat/*.1000; do node $@.js "$$name"; done;

clean:
	rm -rf dat

dat:
	@node setup.js
