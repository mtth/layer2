#include "pcap_wrapper.hpp"
#include <cstring>
#include <node_buffer.h>
#include <sys/ioctl.h>
#include <vector>

#define precondition(b) if (!(b)) return NanThrowError("Illegal arguments.")
#define check_handle_not_null(w) if ((w)->handle == NULL) \
  return NanThrowError("Inactive wrapper.")

using namespace v8;

// Async dispatcher.

class Dispatcher : public NanAsyncWorker {

public:

  Dispatcher(
    NanCallback *callback,
    pcap_t *pcap_handle,
    int batch_size, // Positive only here (for headers initialization).
    u_char *data,
    int break_len, // _After_ which dispatcher will break (-1 for no break).
    int len, // Throws an error if would write past this.
    bool *running // Flag to signal end of dispatching.
  ) : NanAsyncWorker(callback) {

    _running = running;
    *_running = true;

    _pcap_handle = pcap_handle;
    _batch_size = batch_size;
    _cur = data;
    _break = break_len >= 0 ? data + break_len : NULL;
    _end = data + len;
    _i = 0;
    _broke = false;
    _headers = std::vector<struct pcap_pkthdr>(batch_size);

  }

  ~Dispatcher() {} // TODO: do we need to free `_headers`?

  void Execute() {

    int n = pcap_dispatch(
      _pcap_handle,
      _batch_size,
      on_packet,
      (u_char *) this
    );

    if (n == -1) {
      SetErrorMessage(pcap_geterr(_pcap_handle));
    }

  }

  void HandleOKCallback() {

    NanScope();

    // Create packet header objects.
    int i;
    Local<Array> headers = NanNew<Array>(_i);
    for (i = 0; i < _i; i++) {
      struct pcap_pkthdr hdr = _headers[i];
      Local<Object> obj = NanNew<Object>();
      // obj->Set(NanNew<String>("timeVal"), NanNew<Number>(hdr.ts)); TODO
      obj->Set(NanNew<String>("capLen"), NanNew<Number>(hdr.caplen));
      obj->Set(NanNew<String>("len"), NanNew<Number>(hdr.len));
      headers->Set(NanNew<Number>(i), obj);
    }

    Local<Value> argv[] = {
      NanNull(), // Error.
      headers,
      NanNew<Boolean>(_broke)
    };

    *_running = false;
    callback->Call(3, argv);

  }

  void HandleErrorCallback() {

    *_running = false;
    NanAsyncWorker::HandleErrorCallback();

  }

  static void on_packet(
    u_char *etc,
    const struct pcap_pkthdr *header,
    const u_char *packet
  ) {

    Dispatcher *dispatcher = (Dispatcher *) etc;
    dispatcher->_headers[dispatcher->_i++] = *header;
    int copy_length = header->caplen;

    int overflow = dispatcher->_cur + copy_length - dispatcher->_end;
    if (overflow > 0) {
      pcap_breakloop(dispatcher->_pcap_handle);
      dispatcher->SetErrorMessage("Buffer overflow.");
      return;
    }

    memcpy(dispatcher->_cur, packet, copy_length);
    dispatcher->_cur += copy_length;

    if (dispatcher->_break != NULL && dispatcher->_cur >= dispatcher->_break) {
      dispatcher->_broke = true;
      pcap_breakloop(dispatcher->_pcap_handle);
    }

  }

private:

  pcap_t *_pcap_handle;
  int _batch_size;
  u_char *_cur;
  u_char *_break;
  u_char *_end;
  int _i;
  bool _broke;
  bool *_running;
  std::vector<struct pcap_pkthdr> _headers;

};

// Wrapper.

PcapWrapper::PcapWrapper() {

  dump_file_p = NULL;
  dump_handle = NULL;
  handle = NULL;
  buffer_size = 0;
  dispatching = false;

}

PcapWrapper::~PcapWrapper() {}

// Constructor used from JavaScript.

NAN_METHOD(PcapWrapper::init) {

  NanScope();
  precondition(args.Length() == 0);

  PcapWrapper *wrapper = new PcapWrapper();
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

// Add save handle (should be called before dump).

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
  check_handle_not_null(wrapper);

  if (pcap_set_snaplen(wrapper->handle, args[0]->Int32Value())) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }

  NanReturnThis();

}

NAN_METHOD(PcapWrapper::set_rfmon) {

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsBoolean());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  check_handle_not_null(wrapper);

  if (pcap_set_rfmon(wrapper->handle, args[0]->Int32Value())) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }
  NanReturnThis();

}

NAN_METHOD(PcapWrapper::set_promisc) {

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsBoolean());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  check_handle_not_null(wrapper);

  if (pcap_set_promisc(wrapper->handle, args[0]->Int32Value())) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }
  NanReturnThis();

}

NAN_METHOD(PcapWrapper::set_buffersize) {

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsInt32());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  check_handle_not_null(wrapper);
  int buffer_size = args[0]->Int32Value();

  if (pcap_set_buffer_size(wrapper->handle, buffer_size) != 0) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }
  wrapper->buffer_size = buffer_size;
  NanReturnThis();

}

NAN_METHOD(PcapWrapper::set_filter) {

  NanScope();
  precondition(args.Length() == 1 && args[0]->IsString());
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  check_handle_not_null(wrapper);
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
  check_handle_not_null(wrapper);

  int snaplen = pcap_snapshot(wrapper->handle);
  NanReturnValue(NanNew<Integer>(snaplen));

}

NAN_METHOD(PcapWrapper::get_datalink) {

  NanScope();
  precondition(args.Length() == 0);
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  check_handle_not_null(wrapper);

  int link = pcap_datalink(wrapper->handle);
  const char *name = pcap_datalink_val_to_name(link);
  NanReturnValue(NanNew<String>(name == NULL ? "UNKNOWN" : name));

}

NAN_METHOD(PcapWrapper::get_stats) {

  NanScope();
  precondition(args.Length() == 0);
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  check_handle_not_null(wrapper);
  struct pcap_stat ps;

  if (pcap_stats(wrapper->handle, &ps) == -1) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }

  Local<Object> stats_obj = NanNew<Object>();
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
  check_handle_not_null(wrapper);

  if (wrapper->buffer_size == 0) {
    return NanThrowError("Buffer size not set.");
  }

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
  precondition(
    args.Length() == 3 &&
    args[0]->IsInt32() &&
    node::Buffer::HasInstance(args[1]) &&
    args[2]->IsFunction()
  );
  PcapWrapper *wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  check_handle_not_null(wrapper);

  if (wrapper->dispatching) {
    return NanThrowError("Already dispatching.");
  }

  int batch_size = args[0]->Int32Value();
  if (batch_size <= 0) {
    return NanThrowError("Batch size must be positive.");
  }

  Local<Object> buffer_obj = args[1]->ToObject();
  u_char *data = (u_char *) node::Buffer::Data(buffer_obj);
  int length = node::Buffer::Length(buffer_obj);
  NanCallback *callback = new NanCallback(args[2].As<Function>());

  int break_length;
  if (pcap_file(wrapper->handle) == NULL) { // Live capture.
    if (length < wrapper->buffer_size) {
      return NanThrowError("Live dispatch buffer too small.");
    }
    break_length = -1; // We are guaranteed to not fill buffer.
  } else { // Offline capture.
    int snaplen = pcap_snapshot(wrapper->handle);
    if (length < snaplen) {
      return NanThrowError("Offline dispatch buffer too small.");
    }
    break_length = length - snaplen;
  }

  NanAsyncQueueWorker(new Dispatcher(
    callback,
    wrapper->handle,
    batch_size,
    data,
    break_length,
    length,
    &wrapper->dispatching
  ));

  NanReturnUndefined();

}

NAN_METHOD(PcapWrapper::close) {

  NanScope();
  precondition(args.Length() == 0);
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  pcap_close(wrapper->handle);
  if (wrapper->dump_handle != NULL) {
    pcap_dump_close(wrapper->dump_handle);
  }

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
