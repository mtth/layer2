#include "utils.hpp"
#include <pcap/pcap.h>

#define CHECK(b) if (!(b)) return NanThrowError("Illegal arguments.")

namespace Layer2 {

NAN_METHOD(ReadMacAddr) {

  static const char hex[] = "0123456789abcdef";

  NanScope();
  CHECK(
    args.Length() == 2 &&
    node::Buffer::HasInstance(args[0]) &&
    args[1]->IsInt32()
  );

  v8::Local<v8::Object> buf = args[0]->ToObject();
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

  NanReturnValue(NanNew<v8::String>(addr));

}

// Get the first device with an address, pcap_lookupdev() just returns the
// first non-loopback device.
NAN_METHOD(GetDefaultDevice) {

  NanScope();
  CHECK(args.Length() == 0);

  v8::Local<v8::Value> name;
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
  name = NanNew<v8::String>(dev->name);
  pcap_freealldevs(alldevs);
  NanReturnValue(name);

}

NAN_METHOD(GetLinkInfo) {

  NanScope();
  CHECK(
    args.Length() == 1 &&
    args[0]->IsInt32()
  );

  int link = args[0]->Int32Value();
  const char *name = pcap_datalink_val_to_name(link);
  if (name == NULL) {
    NanReturnNull();
  }

  v8::Local<v8::Object> info = NanNew<v8::Object>();
  info->Set(NanNew("name"), NanNew(name));

  const char *description = pcap_datalink_val_to_description(link);
  if (description != NULL) {
    info->Set(NanNew("description"), NanNew(description));
  }

  NanReturnValue(info);

}

} // Layer 2
