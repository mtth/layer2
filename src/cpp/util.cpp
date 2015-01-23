#include "util.hpp"
#include <node_buffer.h>
#include <pcap/pcap.h>

#define precondition(b) if (!(b)) return NanThrowError("Illegal arguments.")

using namespace v8;

static const char hex[] = "0123456789abcdef";

// Parse MAC address.
NAN_METHOD(read_mac_addr) {

  NanScope();
  precondition(
    args.Length() == 2 &&
    node::Buffer::HasInstance(args[0]) &&
    args[1]->IsInt32()
  );

  Local<Object> buf = args[0]->ToObject();
  register unsigned int offset = args[1]->Int32Value();
  if (offset > node::Buffer::Length(buf) - 6) {
    return NanThrowError("Truncated address.");
  }

  register int i = 5;
  register unsigned char *data = (unsigned char *) node::Buffer::Data(buf) + offset;
  char addr[18];
  register char *cp = addr;

  *cp++ = hex[*data >> 4];
  *cp++ = hex[*data++ & 0x0f];
  while (i--) {
    *cp++ = ':';
    *cp++ = hex[*data >> 4];
    *cp++ = hex[*data++ & 0x0f];
  }
  *cp = '\0';

  NanReturnValue(NanNew<String>(addr));

}

// Get the first device with an address, pcap_lookupdev() just returns the
// first non-loopback device.
NAN_METHOD(get_default_dev) {

  NanScope();
  precondition(args.Length() == 0);

  Local<Value> name;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_if_t *alldevs, *dev;
  pcap_addr_t *addr;

  if (pcap_findalldevs(&alldevs, errbuf) == -1) {
    return NanThrowError(errbuf);
  }

  if (alldevs != NULL) {
    for (dev = alldevs; dev != NULL; dev = dev->next) {
      if (dev->addresses != NULL && !(dev->flags & PCAP_IF_LOOPBACK)) {
        for (addr = dev->addresses; addr != NULL; addr = addr->next) {
          if (addr->addr->sa_family == AF_INET) {
            goto found;
          }
        }
      }
    }
    pcap_freealldevs(alldevs);
  }
  NanReturnNull();

found:
  name = NanNew<String>(dev->name);
  pcap_freealldevs(alldevs);
  NanReturnValue(name);

}
