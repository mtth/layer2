#include "pcap_wrapper.hpp"
#include <cstring>
#include <node_buffer.h>
#include <sys/ioctl.h>

#define precondition(b) if (!(b)) return NanThrowError("Illegal arguments.")

using namespace v8;

PcapWrapper::PcapWrapper() {

  dump_file_p = NULL;
  buffer_data = NULL;
  dump_handle = NULL;
  handle = NULL;

}

PcapWrapper::~PcapWrapper() {}

// Constructor used from JavaScript.

NAN_METHOD(PcapWrapper::init) {

  NanScope();
  precondition(args.Length() == 1 && node::Buffer::HasInstance(args[0]));

  PcapWrapper *wrapper = new PcapWrapper();
  Local<Object> buffer_obj = args[0]->ToObject();
  wrapper->buffer_data = node::Buffer::Data(buffer_obj);
  wrapper->buffer_length = node::Buffer::Length(buffer_obj);
  wrapper->buffer_offset = 0;
  wrapper->Wrap(args.This());

  NanReturnThis();

}

// Handle types.

NAN_METHOD(PcapWrapper::from_device) {

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsString());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  char errbuf[PCAP_ERRBUF_SIZE];
  String::Utf8Value device(args[0]->ToString());

  pcap_t *handle = pcap_create((char *) *device, errbuf);
  if (handle == NULL || pcap_setnonblock(handle, 1, errbuf) == -1) {
    return NanThrowError(errbuf);
  }
  wrapper->handle = handle;
  NanReturnThis();

}

NAN_METHOD(PcapWrapper::from_savefile) {

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsString());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  char errbuf[PCAP_ERRBUF_SIZE];
  String::Utf8Value path(args[0]->ToString());
  pcap_t *handle = pcap_open_offline((char *) *path, errbuf);

  if (handle == NULL) {
    return NanThrowError(errbuf);
  }
  wrapper->handle = handle;
  NanReturnThis();

}

NAN_METHOD(PcapWrapper::from_dead) {

  NanScope();
  precondition(args.Length() == 2 && args[0]->IsString() && args[1]->IsInt32());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  // Extract datalink type.
  String::Utf8Value datalink_name(args[0]->ToString());
  int linktype = pcap_datalink_name_to_val((char *) *datalink_name);
  if (linktype == -1) {
    return NanThrowError("Invalid link type.");
  }

  int snaplen = args[1]->Int32Value();
  wrapper->handle = pcap_open_dead(linktype, snaplen);
  NanReturnThis();

}


NAN_METHOD(PcapWrapper::to_savefile) {

  // TODO: add write mode option (e.g. append). This would require skipping the
  // header and doing some link type consistency checks.

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsString());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  String::Utf8Value path(args[0]->ToString());
  wrapper->dump_file_p = fopen((char *) *path, "w");
  if (wrapper->dump_file_p == NULL) {
    return NanThrowError("Can't open file.");
  }

  wrapper->dump_handle = pcap_dump_fopen(wrapper->handle, wrapper->dump_file_p);
  if (wrapper->dump_handle == NULL) {
    fclose(wrapper->dump_file_p);
    return NanThrowError(pcap_geterr(wrapper->handle));
  }

  NanReturnThis();

}

// Configuration (for live captures).

NAN_METHOD(PcapWrapper::set_snaplen) {

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsInt32());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_set_snaplen(wrapper->handle, args[0]->Int32Value())) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }

  NanReturnThis();

}

NAN_METHOD(PcapWrapper::set_rfmon) {

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsBoolean());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_set_rfmon(wrapper->handle, args[0]->Int32Value())) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }
  NanReturnThis();

}

NAN_METHOD(PcapWrapper::set_promisc) {

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsBoolean());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_set_promisc(wrapper->handle, args[0]->Int32Value())) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }
  NanReturnThis();

}

NAN_METHOD(PcapWrapper::set_buffersize) {

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsInt32());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_set_buffer_size(wrapper->handle, args[0]->Int32Value()) != 0) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }
  NanReturnThis();

}

NAN_METHOD(PcapWrapper::set_filter) {

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsString());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  String::Utf8Value filter(args[0]->ToString());

  if (filter.length() != 0) {
    if (pcap_compile(wrapper->handle, &wrapper->filter, (char *) *filter, 1, PCAP_NETMASK_UNKNOWN) == -1) {
      return NanThrowError(pcap_geterr(wrapper->handle));
    }
    if (pcap_setfilter(wrapper->handle, &wrapper->filter) == -1) {
      return NanThrowError(pcap_geterr(wrapper->handle));
    }
    pcap_freecode(&wrapper->filter);
  }
  NanReturnThis();

}

// Helpers.

NAN_METHOD(PcapWrapper::get_snaplen) {

  NanScope();
  precondition(args.Length() == 0);
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  int snaplen = pcap_snapshot(wrapper->handle);
  NanReturnValue(NanNew<Integer>(snaplen));

}

NAN_METHOD(PcapWrapper::get_datalink) {

  NanScope();
  precondition(args.Length() == 0);
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  int link = pcap_datalink(wrapper->handle);
  const char *name = pcap_datalink_val_to_name(link);
  NanReturnValue(NanNew<String>(name == NULL ? "UNKNOWN" : name));

}

NAN_METHOD(PcapWrapper::get_stats) {
  NanScope();

  struct pcap_stat ps;
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_stats(wrapper->handle, &ps) == -1) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }

  Local<Object> stats_obj = Object::New();
  stats_obj->Set(NanNew<String>("psRecv"), NanNew<Integer>(ps.ps_recv));
  stats_obj->Set(NanNew<String>("psDrop"), NanNew<Integer>(ps.ps_drop));
  stats_obj->Set(NanNew<String>("psIfDrop"), NanNew<Integer>(ps.ps_ifdrop));
  // ps_ifdrop may not be supported on this platform, but there's no good way
  // to tell, is there?

  NanReturnValue(stats_obj);

}

// Start the fun.

NAN_METHOD(PcapWrapper::activate) {

  NanScope();
  precondition(args.Length() == 0);
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (pcap_activate(wrapper->handle)) {
    return NanThrowError(pcap_geterr(wrapper->handle));
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
    return NanThrowError("Can't set device to non-blocking mode.");
  }
#endif

  NanReturnThis();

}

NAN_METHOD(PcapWrapper::dispatch) {

  NanScope();
  precondition(args.Length() == 2 && args[0]->IsInt32() && args[1]->IsFunction());
  PcapWrapper *wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  wrapper->on_packet_callback = Persistent<Function>::New(Handle<Function>::Cast(args[1]));

  wrapper->buffer_offset = 0;
  int n = pcap_dispatch(wrapper->handle, args[0]->Int32Value(), on_packet, (u_char *) wrapper);
  if (n == -1) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }
  NanReturnValue(NanNew<Integer>(n));

}

NAN_METHOD(PcapWrapper::break_loop) {

  NanScope();
  precondition(args.Length() == 0);
  PcapWrapper *wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  pcap_breakloop(wrapper->handle);
  NanReturnThis();

}

NAN_METHOD(PcapWrapper::close) {

  NanScope();
  precondition(args.Length() == 0);
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  pcap_close(wrapper->handle);
  if (wrapper->dump_handle != NULL) {
    pcap_dump_close(wrapper->dump_handle);
  }
  wrapper->on_packet_callback.Dispose();

  NanReturnUndefined();

}

NAN_METHOD(PcapWrapper::inject) {

  NanScope();
  precondition(args.Length() == 1 && node::Buffer::HasInstance(args[0]));
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  Local<Object> packet = args[0]->ToObject();
  char *packet_data = node::Buffer::Data(packet);
  int packet_length = node::Buffer::Length(packet);

  int n = pcap_inject(wrapper->handle, packet_data, packet_length);
  if (n == -1) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  } else if (n != packet_length) {
    return NanThrowError("Frame injection truncated.");
  }

  NanReturnThis();

}

NAN_METHOD(PcapWrapper::dump) {

  NanScope();
  precondition(args.Length() == 1 && node::Buffer::HasInstance(args[0]));
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  if (wrapper->dump_handle == NULL) {
    return NanThrowError("No writable savefile active.");
  }

  struct timeval tv;
  gettimeofday(&tv, NULL);

  Local<Object> packet = args[0]->ToObject();
  char *packet_data = node::Buffer::Data(packet);
  int packet_length = node::Buffer::Length(packet);

  // Create fake packet header.
  int snaplen = pcap_snapshot(wrapper->handle);
  struct pcap_pkthdr pktheader = {
    tv, // ts
    packet_length > snaplen ? snaplen : packet_length, // caplen
    packet_length, // len
#ifdef __APPLE__
    "Length might be wrong if the frame was already truncated." // comment
#endif
  };

  pcap_dump((u_char *) wrapper->dump_handle, &pktheader, (u_char *) packet_data);

  NanReturnThis();

}

void PcapWrapper::on_packet(
  u_char *reader_p,
  const struct pcap_pkthdr* pkthdr,
  const u_char* packet
) {

  NanScope();
  PcapWrapper *wrapper = (PcapWrapper *) reader_p;

  int packet_overflow = pkthdr->len - pkthdr->caplen;
  int overflow = wrapper->buffer_offset + pkthdr->caplen - wrapper->buffer_length;
  size_t copy_length = pkthdr->caplen - (overflow > 0 ? overflow : 0);
  memcpy(wrapper->buffer_data + wrapper->buffer_offset, packet, copy_length);
  wrapper->buffer_offset += copy_length;

  TryCatch try_catch;

  Local<Value> argv[3] = {
    NanNew<Integer>(wrapper->buffer_offset), // New offset.
    NanNew<Integer>(overflow), // Buffer overflow.
    NanNew<Integer>(packet_overflow) // Frame overflow.
  };
  wrapper->on_packet_callback->Call(Context::GetCurrent()->Global(), 3, argv);

  if (try_catch.HasCaught())  {
    node::FatalException(try_catch);
  }

}

// Registration.

void pcap_wrapper_expose(Handle<Object> exports) {

  char className[] = "PcapWrapper";

  // Prepare constructor template
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(PcapWrapper::init);
  tpl->SetClassName(NanNew<String>(className));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype

  #define add_to_prototype(t, s, m) NanSetPrototypeTemplate( \
    (t), \
    (s), \
    NanNew<FunctionTemplate>((m))->GetFunction() \
  )
  add_to_prototype(tpl, "activate", PcapWrapper::activate);
  add_to_prototype(tpl, "breakLoop", PcapWrapper::break_loop);
  add_to_prototype(tpl, "close", PcapWrapper::close);
  add_to_prototype(tpl, "dispatch", PcapWrapper::dispatch);
  add_to_prototype(tpl, "dumpFrame", PcapWrapper::dump);
  add_to_prototype(tpl, "fromDead", PcapWrapper::from_dead);
  add_to_prototype(tpl, "fromDevice", PcapWrapper::from_device);
  add_to_prototype(tpl, "fromSavefile", PcapWrapper::from_savefile);
  add_to_prototype(tpl, "getLinkType", PcapWrapper::get_datalink);
  add_to_prototype(tpl, "getMaxFrameSize", PcapWrapper::get_snaplen);
  add_to_prototype(tpl, "getStats", PcapWrapper::get_stats);
  add_to_prototype(tpl, "injectFrame", PcapWrapper::inject);
  add_to_prototype(tpl, "setBufferSize", PcapWrapper::set_buffersize);
  add_to_prototype(tpl, "setFilter", PcapWrapper::set_filter);
  add_to_prototype(tpl, "setPromisc", PcapWrapper::set_promisc);
  add_to_prototype(tpl, "setMonitor", PcapWrapper::set_rfmon);
  add_to_prototype(tpl, "setMaxFrameSize", PcapWrapper::set_snaplen);
  add_to_prototype(tpl, "toSavefile", PcapWrapper::to_savefile);
  #undef add_to_prototype

  exports->Set(NanNew<String>(className), tpl->GetFunction());

}
