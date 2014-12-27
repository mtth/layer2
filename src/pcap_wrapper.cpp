#include <node_buffer.h>
#include <node_version.h>
#include <sys/ioctl.h>
#include <cstring>
#include <string.h>
#include "pcap_wrapper.h"

#define precondition(b) \
  if (!(b)) \
  return ThrowException(Exception::TypeError(String::New("Illegal arguments.")))
#define add_to_prototype(tpl, s, t) \
  (tpl)->PrototypeTemplate()->Set( \
    String::NewSymbol(t), \
    FunctionTemplate::New(s)->GetFunction() \
  )


using namespace v8;

PcapWrapper::PcapWrapper() {

  dump_file_p = NULL;
  buffer_data = NULL;
  dump_handle = NULL;
  handle = NULL;

}

PcapWrapper::~PcapWrapper() {}

// Constructor used from JavaScript.

Handle<Value> PcapWrapper::init(const Arguments& args) {

  precondition(args.Length() == 1 && node::Buffer::HasInstance(args[0]));
  HandleScope scope;

  PcapWrapper *wrapper = new PcapWrapper();
  Local<Object> buffer_obj = args[0]->ToObject();
  wrapper->buffer_data = node::Buffer::Data(buffer_obj);
  wrapper->buffer_length = node::Buffer::Length(buffer_obj);
  wrapper->buffer_offset = 0;
  wrapper->Wrap(args.This());
  return args.This();

}


// Handle types.

Handle<Value> PcapWrapper::from_device(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsString());
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  char errbuf[PCAP_ERRBUF_SIZE];
  String::Utf8Value device(args[0]->ToString());

  pcap_t *handle = pcap_create((char *) *device, errbuf);
  if (handle == NULL || pcap_setnonblock(handle, 1, errbuf) == -1) {
    return ThrowException(Exception::Error(String::New(errbuf)));
  }
  wrapper->handle = handle;
  return args.This();

}

Handle<Value> PcapWrapper::from_savefile(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsString());
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  char errbuf[PCAP_ERRBUF_SIZE];
  String::Utf8Value path(args[0]->ToString());
  pcap_t *handle = pcap_open_offline((char *) *path, errbuf);

  if (handle == NULL) {
    return ThrowException(Exception::Error(String::New(errbuf)));
  }
  wrapper->handle = handle;
  return args.This();

}

Handle<Value> PcapWrapper::from_dead(const Arguments& args) {

  precondition(args.Length() == 2 && args[0]->IsString() && args[1]->IsInt32());
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  // Extract datalink type.
  String::Utf8Value datalink_name(args[0]->ToString());
  int linktype = pcap_datalink_name_to_val((char *) *datalink_name);
  if (linktype == -1) {
    return ThrowException(Exception::Error(String::New("Invalid link type.")));
  }

  int snaplen = args[1]->Int32Value();
  wrapper->handle = pcap_open_dead(linktype, snaplen);

  return args.This();

}


Handle<Value> PcapWrapper::to_savefile(const Arguments& args) {

  // TODO: add write mode option (e.g. append). This would require skipping the
  // header and doing some link type consistency checks.

  precondition(args.Length() == 1 && args[0]->IsString());
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  String::Utf8Value path(args[0]->ToString());
  wrapper->dump_file_p = fopen((char *) *path, "w");
  if (wrapper->dump_file_p == NULL) {
    return ThrowException(Exception::Error(String::New("Can't open file.")));
  }

  wrapper->dump_handle = pcap_dump_fopen(wrapper->handle, wrapper->dump_file_p);
  if (wrapper->dump_handle == NULL) {
    fclose(wrapper->dump_file_p);
    return ThrowException(Exception::Error(String::New(pcap_geterr(wrapper->handle))));
  }

  return args.This();

}

// Configuration (for live captures).

Handle<Value> PcapWrapper::set_snaplen(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsInt32());
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_set_snaplen(wrapper->handle, args[0]->Int32Value())) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(wrapper->handle))));
  }

  return args.This();

}

Handle<Value> PcapWrapper::set_rfmon(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsBoolean());
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_set_rfmon(wrapper->handle, args[0]->Int32Value())) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(wrapper->handle))));
  }
  return args.This();

}

Handle<Value> PcapWrapper::set_promisc(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsBoolean());
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_set_promisc(wrapper->handle, args[0]->Int32Value())) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(wrapper->handle))));
  }
  return args.This();

}

Handle<Value> PcapWrapper::set_buffersize(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsInt32());
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_set_buffer_size(wrapper->handle, args[0]->Int32Value()) != 0) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(wrapper->handle))));
  }
  return args.This();

}

Handle<Value> PcapWrapper::set_filter(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsString());
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  String::Utf8Value filter(args[0]->ToString());

  if (filter.length() != 0) {
    if (pcap_compile(wrapper->handle, &wrapper->filter, (char *) *filter, 1, PCAP_NETMASK_UNKNOWN) == -1) {
      return ThrowException(Exception::Error(String::New(pcap_geterr(wrapper->handle))));
    }
    if (pcap_setfilter(wrapper->handle, &wrapper->filter) == -1) {
      return ThrowException(Exception::Error(String::New(pcap_geterr(wrapper->handle))));
    }
    pcap_freecode(&wrapper->filter);
  }
  return args.This();

}

// Helpers.

Handle<Value> PcapWrapper::get_snaplen(const Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  int snaplen = pcap_snapshot(wrapper->handle);
  Local<Value> wrapped = Integer::New(snaplen);
  return scope.Close(wrapped);

}

Handle<Value> PcapWrapper::get_datalink(const Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  int link = pcap_datalink(wrapper->handle);
  const char *name = pcap_datalink_val_to_name(link);
  Local<Value> wrapped = String::New(name == NULL ? "UNKNOWN" : name);
  return scope.Close(wrapped);

}

Handle<Value> PcapWrapper::get_stats(const Arguments& args) {

  HandleScope scope;
  struct pcap_stat ps;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_stats(wrapper->handle, &ps) == -1) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(wrapper->handle))));
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

Handle<Value> PcapWrapper::activate(const Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_activate(wrapper->handle)) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(wrapper->handle))));
  }

#if defined(__APPLE_CC__) || defined(__APPLE__)
  // Work around buffering bug in BPF on OSX 10.6 as of May 19, 2010 This may
  // result in dropped packets under load because it disables the (broken)
  // buffer
  // http://seclists.org/tcpdump/2010/q1/110
  #include <net/bpf.h>
  int fd = pcap_get_selectable_fd(wrapper->handle);
  int v = 1;
  if (ioctl(fd, BIOCIMMEDIATE, &v) == -1) {
    return ThrowException(Exception::Error(String::New("Can't set device to non-blocking mode.")));
  }
#else
  char errbuf[PCAP_ERRBUF_SIZE];
  if (pcap_setnonblock(wrapper->handle, 1, errbuf) == -1) {
    return ThrowException(Exception::Error(String::New(errbuf)));
  }
#endif

  return args.This();

}

Handle<Value> PcapWrapper::dispatch(const Arguments& args) {

  precondition(args.Length() == 2 && args[0]->IsInt32() && args[1]->IsFunction());
  HandleScope scope;
  PcapWrapper *wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  wrapper->on_packet_callback = Persistent<Function>::New(Handle<Function>::Cast(args[1]));

  wrapper->buffer_offset = 0;
  int n = pcap_dispatch(wrapper->handle, args[0]->Int32Value(), on_packet, (u_char *) wrapper);
  if (n == -1) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(wrapper->handle))));
  }
  return scope.Close(Integer::New(n));

}

Handle<Value> PcapWrapper::break_loop(const Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;
  PcapWrapper *wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  pcap_breakloop(wrapper->handle);
  return args.This();

}

Handle<Value> PcapWrapper::close(const Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  pcap_close(wrapper->handle);
  if (wrapper->dump_handle != NULL) {
    pcap_dump_close(wrapper->dump_handle);
  }
  wrapper->on_packet_callback.Dispose();

  return Undefined();

}

Handle<Value> PcapWrapper::inject(const Arguments& args) {

  precondition(args.Length() == 1 && node::Buffer::HasInstance(args[0]));
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  Local<Object> packet = args[0]->ToObject();
  char *packet_data = node::Buffer::Data(packet);
  int packet_length = node::Buffer::Length(packet);

  int n = pcap_inject(wrapper->handle, packet_data, packet_length);
  if (n == -1) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(wrapper->handle))));
  } else if (n != packet_length) {
    return ThrowException(Exception::Error(String::New("Packet injection truncated.")));
  }

  return args.This();

}

Handle<Value> PcapWrapper::dump(const Arguments& args) {

  precondition(args.Length() == 1 && node::Buffer::HasInstance(args[0]));
  HandleScope scope;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (wrapper->dump_handle == NULL) {
      return ThrowException(Exception::Error(String::New("No writable savefile active.")));
  }

  struct timeval tv;
  gettimeofday(&tv, NULL);

  Local<Object> packet = args[0]->ToObject();
  char *packet_data = node::Buffer::Data(packet);
  int packet_length = node::Buffer::Length(packet);

  // Create fake packet header.
  int snaplen = pcap_snapshot(wrapper->handle);
  struct pcap_pkthdr pktheader = {};
  pktheader.ts = tv;
  pktheader.caplen = packet_length > snaplen ? snaplen : packet_length;
  pktheader.len = packet_length; // WARNING: this might be wrong.

  pcap_dump((u_char *) wrapper->dump_handle, &pktheader, (u_char *) packet_data);

  return args.This();

}

void PcapWrapper::on_packet(
  u_char *reader_p,
  const struct pcap_pkthdr* pkthdr,
  const u_char* packet
) {

  HandleScope scope;
  PcapWrapper *wrapper = (PcapWrapper *) reader_p;

  int packet_overflow = pkthdr->len - pkthdr->caplen;
  int overflow = wrapper->buffer_offset + pkthdr->caplen - wrapper->buffer_length;
  size_t copy_length = pkthdr->caplen - (overflow > 0 ? overflow : 0);
  memcpy(wrapper->buffer_data + wrapper->buffer_offset, packet, copy_length);
  wrapper->buffer_offset += copy_length;

  TryCatch try_catch;

  Local<Value> argv[3] = {
    Integer::New(wrapper->buffer_offset), // New offset.
    Integer::New(overflow), // Buffer overflow.
    Integer::New(packet_overflow) // Packet overflow.
  };
  wrapper->on_packet_callback->Call(Context::GetCurrent()->Global(), 3, argv);

  if (try_catch.HasCaught())  {
    node::FatalException(try_catch);
  }

}

// Registration.

void wrapper_expose(Handle<Object> exports) {

  char className[] = "Wrapper";

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(PcapWrapper::init);
  tpl->SetClassName(String::NewSymbol(className));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  add_to_prototype(tpl, PcapWrapper::activate, "activate");
  add_to_prototype(tpl, PcapWrapper::break_loop, "breakLoop");
  add_to_prototype(tpl, PcapWrapper::close, "close");
  add_to_prototype(tpl, PcapWrapper::dispatch, "dispatch");
  add_to_prototype(tpl, PcapWrapper::dump, "dumpPacket");
  add_to_prototype(tpl, PcapWrapper::from_dead, "fromDead");
  add_to_prototype(tpl, PcapWrapper::from_device, "fromDevice");
  add_to_prototype(tpl, PcapWrapper::from_savefile, "fromSavefile");
  add_to_prototype(tpl, PcapWrapper::get_datalink, "getLinkType");
  add_to_prototype(tpl, PcapWrapper::get_snaplen, "getMaxPacketSize");
  add_to_prototype(tpl, PcapWrapper::get_stats, "getStats");
  add_to_prototype(tpl, PcapWrapper::inject, "injectPacket");
  add_to_prototype(tpl, PcapWrapper::set_buffersize, "setBufferSize");
  add_to_prototype(tpl, PcapWrapper::set_filter, "setFilter");
  add_to_prototype(tpl, PcapWrapper::set_promisc, "setPromisc");
  add_to_prototype(tpl, PcapWrapper::set_rfmon, "setMonitor");
  add_to_prototype(tpl, PcapWrapper::set_snaplen, "setMaxPacketSize");
  add_to_prototype(tpl, PcapWrapper::to_savefile, "toSavefile");

  exports->Set(
    String::NewSymbol(className),
    Persistent<Function>::New(tpl->GetFunction())
  );

}
