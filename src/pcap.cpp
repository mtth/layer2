#include <node_buffer.h>
#include <node_version.h>
#include <sys/ioctl.h>
#include <cstring>
#include <string.h>
#include "pcap.h"

#define precondition(b) \
  if (!(b)) \
  return ThrowException(Exception::TypeError(String::New("Illegal arguments.")))
#define add_to_prototype(tpl, s) \
  (tpl)->PrototypeTemplate()->Set( \
    String::NewSymbol(#s), \
    FunctionTemplate::New(s)->GetFunction() \
  )


using namespace v8;

Pcap::Pcap() {}

Pcap::~Pcap() {}

// Constructor used from JavaScript.

Handle<Value> Pcap::init(const Arguments& args) {

  precondition(args.Length() == 1 && node::Buffer::HasInstance(args[0]));
  HandleScope scope;

  Pcap *pcap = new Pcap();
  Local<Object> buffer_obj = args[0]->ToObject();
  pcap->buffer_data = node::Buffer::Data(buffer_obj);
  pcap->buffer_length = node::Buffer::Length(buffer_obj);
  pcap->buffer_offset = 0;
  pcap->Wrap(args.This());
  return args.This();

}


// Handle types.

Handle<Value> Pcap::fromDevice(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsString());
  HandleScope scope;
  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());
  char errbuf[PCAP_ERRBUF_SIZE];
  String::Utf8Value device(args[0]->ToString());

  pcap_t *handle = pcap_create((char *) *device, errbuf);
  if (handle == NULL || pcap_setnonblock(handle, 1, errbuf) == -1) {
    return ThrowException(Exception::Error(String::New(errbuf)));
  }
  pcap->handle = handle;
  return args.This();

}

Handle<Value> Pcap::fromSavefile(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsString());
  HandleScope scope;
  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());
  char errbuf[PCAP_ERRBUF_SIZE];
  String::Utf8Value path(args[0]->ToString());
  pcap_t *handle = pcap_open_offline((char *) *path, errbuf);

  if (handle == NULL) {
    return ThrowException(Exception::Error(String::New(errbuf)));
  }
  pcap->handle = handle;
  return args.This();

}

// Configuration (for live captures).

Handle<Value> Pcap::setSnapLen(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsInt32());
  HandleScope scope;
  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());

  if (pcap_set_snaplen(pcap->handle, args[0]->Int32Value())) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(pcap->handle))));
  }
  return args.This();

}

Handle<Value> Pcap::setRfMon(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsBoolean());
  HandleScope scope;
  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());

  if (pcap_set_rfmon(pcap->handle, args[0]->Int32Value())) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(pcap->handle))));
  }
  return args.This();

}

Handle<Value> Pcap::setPromisc(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsBoolean());
  HandleScope scope;
  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());

  if (pcap_set_promisc(pcap->handle, args[0]->Int32Value())) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(pcap->handle))));
  }
  return args.This();

}

Handle<Value> Pcap::setBufferSize(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsInt32());
  HandleScope scope;
  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());

  if (pcap_set_buffer_size(pcap->handle, args[0]->Int32Value()) != 0) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(pcap->handle))));
  }
  return args.This();

}

Handle<Value> Pcap::setFilter(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsString());
  HandleScope scope;
  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());
  String::Utf8Value filter(args[0]->ToString());

  if (filter.length() != 0) {
    if (pcap_compile(pcap->handle, &pcap->filter, (char *) *filter, 1, PCAP_NETMASK_UNKNOWN) == -1) {
      return ThrowException(Exception::Error(String::New(pcap_geterr(pcap->handle))));
    }
    if (pcap_setfilter(pcap->handle, &pcap->filter) == -1) {
      return ThrowException(Exception::Error(String::New(pcap_geterr(pcap->handle))));
    }
    pcap_freecode(&pcap->filter);
  }
  return args.This();

}

// Helpers.

Handle<Value> Pcap::getDatalink(const Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;
  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());
  int link = pcap_datalink(pcap->handle);
  const char *name = pcap_datalink_val_to_name(link);
  Local<Value> wrapped = String::New(name == NULL ? "UNKNOWN" : name);
  return scope.Close(wrapped);

}

Handle<Value> Pcap::stats(const Arguments& args) {

  HandleScope scope;
  struct pcap_stat ps;

  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());

  if (pcap_stats(pcap->handle, &ps) == -1) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(pcap->handle))));
  }

  Local<Object> stats_obj = Object::New();
  stats_obj->Set(String::New("psRecv"), Integer::NewFromUnsigned(ps.ps_recv));
  stats_obj->Set(String::New("psDrop"), Integer::NewFromUnsigned(ps.ps_drop));
  stats_obj->Set(String::New("psIfDrop"), Integer::NewFromUnsigned(ps.ps_ifdrop));
  // ps_ifdrop may not be supported on this platform, but there's no good way
  // to tell, is there?

  return scope.Close(stats_obj);

}

// Start the fun.

Handle<Value> Pcap::activate(const Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;
  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());
  if (pcap_activate(pcap->handle)) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(pcap->handle))));
  }

  // Work around buffering bug in BPF on OSX 10.6 as of May 19, 2010 This may
  // result in dropped packets under load because it disables the (broken)
  // buffer
  // http://seclists.org/tcpdump/2010/q1/110
#if defined(__APPLE_CC__) || defined(__APPLE__)
  #include <net/bpf.h>
  int fd = pcap_get_selectable_fd(pcap->handle);
  int v = 1;
  ioctl(fd, BIOCIMMEDIATE, &v);
  // TODO - check return value
#endif

  return args.This();

}

Handle<Value> Pcap::dispatch(const Arguments& args) {

  precondition(args.Length() == 2 && args[0]->IsInt32() && args[1]->IsFunction());
  HandleScope scope;
  Pcap *pcap = ObjectWrap::Unwrap<Pcap>(args.This());
  pcap->packet_ready_cb = Persistent<Function>::New(Handle<Function>::Cast(args[1]));

  pcap->buffer_offset = 0;
  int n = pcap_dispatch(pcap->handle, args[0]->Int32Value(), onPacket, (u_char *) pcap);
  if (n == -1) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(pcap->handle))));
  }
  return scope.Close(Integer::New(n));

}

Handle<Value> Pcap::breakLoop(const Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;
  Pcap *pcap = ObjectWrap::Unwrap<Pcap>(args.This());

  pcap_breakloop(pcap->handle);
  return args.This();

}

Handle<Value> Pcap::close(const Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;
  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());

  pcap_close(pcap->handle);
  pcap->packet_ready_cb.Dispose();
  return Undefined();

}

Handle<Value> Pcap::inject(const Arguments& args) {

  HandleScope scope;
  if (args.Length() != 1) {
      return ThrowException(Exception::TypeError(String::New("Inject takes exactly one argument")));
  }
  if (!node::Buffer::HasInstance(args[0])) {
      return ThrowException(Exception::TypeError(String::New("First argument must be a buffer")));
  }

  Pcap* pcap = ObjectWrap::Unwrap<Pcap>(args.This());
  char * bufferData = NULL;
  size_t bufferLength = 0;
  Local<Object> buffer_obj = args[0]->ToObject();
  bufferData = node::Buffer::Data(buffer_obj);
  bufferLength = node::Buffer::Length(buffer_obj);

  if (pcap_inject(pcap->handle, bufferData, bufferLength) != (int)bufferLength) {
      return ThrowException(Exception::Error(String::New("Pcap inject failed.")));
  }

  return args.This();

}

void Pcap::onPacket(
  u_char *s,
  const struct pcap_pkthdr* pkthdr,
  const u_char* packet
) {

  HandleScope scope;
  Pcap *pcap = (Pcap *) s;

  int packetOverflow = pkthdr->len - pkthdr->caplen;
  int overflow = pcap->buffer_offset + pkthdr->caplen - pcap->buffer_length;
  overflow = overflow > 0 ? overflow : 0;
  size_t copy_length = pkthdr->caplen - overflow;
  memcpy(pcap->buffer_data + pcap->buffer_offset, packet, copy_length);
  pcap->buffer_offset += copy_length;

  TryCatch try_catch;

  Local<Value> argv[3] = {
    Integer::New(pcap->buffer_offset), // New offset.
    Integer::New(overflow), // Buffer overflow.
    Integer::New(packetOverflow) // Packet overflow.
  };
  pcap->packet_ready_cb->Call(Context::GetCurrent()->Global(), 3, argv);

  if (try_catch.HasCaught())  {
    node::FatalException(try_catch);
  }

}

// Registration.

void Pcap::init(Handle<Object> exports) {

  char className[] = "Pcap";

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(init);
  tpl->SetClassName(String::NewSymbol(className));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  add_to_prototype(tpl, activate);
  add_to_prototype(tpl, breakLoop);
  add_to_prototype(tpl, close);
  add_to_prototype(tpl, dispatch);
  add_to_prototype(tpl, fromDevice);
  add_to_prototype(tpl, fromSavefile);
  add_to_prototype(tpl, getDatalink);
  add_to_prototype(tpl, setBufferSize);
  add_to_prototype(tpl, setFilter);
  add_to_prototype(tpl, setPromisc);
  add_to_prototype(tpl, setRfMon);
  add_to_prototype(tpl, setSnapLen);
  add_to_prototype(tpl, stats);

  exports->Set(
    String::NewSymbol(className),
    Persistent<Function>::New(tpl->GetFunction())
  );

}
