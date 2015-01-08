#include "util.h"
#include <node_buffer.h>

#define precondition(b) \
  if (!(b)) \
  return ThrowException(Exception::TypeError(String::New("Illegal arguments.")))


using namespace v8;


static const char hex[] = "0123456789abcdef";

// Parse MAC address.
v8::Handle<v8::Value> read_mac_addr(const v8::Arguments& args) {

  precondition(
    args.Length() == 2 &&
    node::Buffer::HasInstance(args[0]) &&
    args[1]->IsInt32()
  );
  HandleScope scope;

  Local<Object> buf = args[0]->ToObject();
  register unsigned int offset = args[1]->Int32Value();
  if (offset > node::Buffer::Length(buf) - 6) {
    return ThrowException(Exception::Error(String::New("Truncated address.")));
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

  Local<Value> wrapped = String::New(addr);
  return scope.Close(wrapped);

}

// Get the first device with an address, pcap_lookupdev() just returns the
// first non-loopback device.
v8::Handle<v8::Value> get_default_dev(const v8::Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;

  char errbuf[PCAP_ERRBUF_SIZE];
  Local<Value> ret;
  pcap_if_t *alldevs, *dev;
  pcap_addr_t *addr;
  bool found = false;

  if (pcap_findalldevs(&alldevs, errbuf) == -1) {
    return ThrowException(Exception::Error(String::New(errbuf)));
  }

  if (alldevs == NULL) {
    return Null();
  }

  for (dev = alldevs; dev != NULL; dev = dev->next) {
    if (dev->addresses != NULL && !(dev->flags & PCAP_IF_LOOPBACK)) {
      for (addr = dev->addresses; addr != NULL; addr = addr->next) {
        if (addr->addr->sa_family == AF_INET) {
          ret = String::New(dev->name);
          found = true;
          break;
        }
      }
      if (found) {
        break;
      }
    }
  }

  pcap_freealldevs(alldevs);
  return scope.Close(ret);

}


// Registration.

void util_expose(Handle<Object> exports) {

  exports->Set(
    String::NewSymbol("getDefaultDevice"),
    FunctionTemplate::New(get_default_dev)->GetFunction()
  );

  exports->Set(
    String::NewSymbol("readMacAddr"),
    FunctionTemplate::New(read_mac_addr)->GetFunction()
  );

}
