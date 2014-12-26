#include <node_buffer.h>
#include <node_version.h>
#include <sys/ioctl.h>
#include <cstring>
#include <string.h>
#include "pcap_util.h"

#define precondition(b) \
  if (!(b)) \
  return ThrowException(Exception::TypeError(String::New("Illegal arguments.")))


using namespace v8;

// Look up the first device with an address, pcap_lookupdev() just returns the
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

v8::Handle<v8::Value> find_all_devs(const v8::Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;

  pcap_if_t *devs, *cur_dev;
  int i;

  char errbuf[PCAP_ERRBUF_SIZE];
  if (pcap_findalldevs(&devs, errbuf) == -1) {
    return ThrowException(Exception::TypeError(String::New(errbuf)));
  }

  Local<Array> ret = Array::New();
  for (i = 0, cur_dev = devs ; cur_dev != NULL ; cur_dev = cur_dev->next, i++) {
    Local<Object> dev = Object::New();
    dev->Set(String::New("name"), String::New(cur_dev->name));
    if (cur_dev->description != NULL) {
      dev->Set(String::New("description"), String::New(cur_dev->description));
    }
    ret->Set(Integer::New(i), dev);
  }

  pcap_freealldevs(devs);
  return scope.Close(ret);

}


// Registration.

void util_expose(Handle<Object> exports) {

  exports->Set(
    String::NewSymbol("getDefaultDevice"),
    FunctionTemplate::New(get_default_dev)->GetFunction()
  );

  exports->Set(
    String::NewSymbol("getAllDevices"),
    FunctionTemplate::New(find_all_devs)->GetFunction()
  );

}
