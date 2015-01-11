#include "pcap_wrapper.hpp"
#include <cstring>
#include <node_buffer.h>
#include <sys/ioctl.h>

#define MAX_FRAME_SIZE 65535

#define precondition(b) if (!(b)) return NanThrowError("Illegal arguments.")
#define check_handle_not_null(w) if ((w)->handle == NULL) \
  return NanThrowError("Inactive capture handle.")

using namespace v8;

// Async dispatcher.

struct safety_buffer {
  bool valid;
  pcap_pkthdr header;
  u_char data[MAX_FRAME_SIZE];
};

class Dispatcher : public NanAsyncWorker {

public:

  Dispatcher(
    NanCallback *callback,
    pcap_t *pcap_handle,
    int batch_size,
    u_char *data,
    int len, // When to break.
    std::vector<struct pcap_pkthdr> &headers, // Where to store headers.
    bool *running // Flag to signal end of dispatching.
  ) : NanAsyncWorker(callback) {

    _running = running;
    *_running = true;

    _pcap_handle = pcap_handle;
    _batch_size = batch_size;
    _stt = data;
    _cur = data;
    _end = data + len;
    _headers = headers;
    _saf.valid = false;

  }

  ~Dispatcher() {}

  void Execute() {

    int n = pcap_dispatch(
      _pcap_handle,
      _batch_size,
      on_packet,
      (u_char *) this
    );
    if (n == -2) {
      // Previous loop was broken, needed to clear flag. We try again.
      n = pcap_dispatch(
        _pcap_handle,
        _batch_size,
        on_packet,
        (u_char *) this
      );
    }

    // Note that we don't propagate the return value of the dispatch call. When
    // reading a file, we can't rely on the output value of it anyway so we
    // always rely on the length of the headers vector instead.

    if (n == -1) {
      SetErrorMessage(pcap_geterr(_pcap_handle));
    }

  }

  void HandleOKCallback() {

    NanScope();
    *_running = false;

    int offset = 0;
    int l = _headers.size();

    Local<Value> argv[] = {
      NanNull(), // Error.
      NanNew<Integer>(_saf.valid ? l + 1 : l), // Start offset.
      NanNew<Integer>(0) // End offset.
    };

    // Flag call (start offset means total number of frames in this case).
    callback->Call(3, argv);

    // Normal frame calls.
    int i;
    for (i = 0; i < l; i++) {
      struct pcap_pkthdr hdr = _headers[i];
      argv[1] = argv[2];
      argv[2] = NanNew<Integer>(offset + hdr.caplen);
      callback->Call(3, argv);
      offset += hdr.caplen;
    }

    if (_saf.valid) {
      // Need to emit the last frame.
      int copy_length = _saf.header.caplen;
      argv[1] = NanNew<Integer>(0);
      argv[2] = NanNew<Integer>(copy_length);
      memcpy(_stt, _saf.data, copy_length);
      callback->Call(3, argv);
    }

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
    int copy_length = header->caplen;

    int overflow = dispatcher->_cur + copy_length - dispatcher->_end;
    if (overflow <= 0) {
      // Usual case.
      dispatcher->_headers.push_back(*header);
      memcpy(dispatcher->_cur, packet, copy_length);
      dispatcher->_cur += copy_length;
    } else {
      // Buffer is full. Break loop and save this frame in our safety buffer.
      dispatcher->_saf.valid = true;
      dispatcher->_saf.header = *header;
      memcpy(dispatcher->_saf.data, packet, copy_length);
      pcap_breakloop(dispatcher->_pcap_handle);
    }

  }

private:

  pcap_t *_pcap_handle;
  int _batch_size;
  u_char *_stt;
  u_char *_cur;
  u_char *_end;
  std::vector<struct pcap_pkthdr> _headers;
  bool *_running;
  struct safety_buffer _saf;

};

// Wrapper.

PcapWrapper::PcapWrapper() {

  dump_file_p = NULL;
  buffer_data = NULL;
  dump_handle = NULL;
  handle = NULL;
  on_packet_callback = NULL;
  dispatching = false;
  headers = std::vector<struct pcap_pkthdr>();

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

  if (wrapper->dispatching) {
    return NanThrowError("Already dispatching.");
  }

  Local<Object> buffer_obj = args[1]->ToObject();
  wrapper->buffer_data = node::Buffer::Data(buffer_obj);
  wrapper->buffer_length = node::Buffer::Length(buffer_obj);
  wrapper->buffer_offset = 0;

  if (wrapper->buffer_length < pcap_snapshot(wrapper->handle)) {
    return NanThrowError("Dispatch buffer too small.");
  }

  Local<Function> fn = args[2].As<Function>();
  wrapper->on_packet_callback = new NanCallback(fn);

  int n = pcap_dispatch(wrapper->handle, args[0]->Int32Value(), on_packet, (u_char *) wrapper);
  if (n == -1) {
    return NanThrowError(pcap_geterr(wrapper->handle));
  }
  NanReturnValue(NanNew<Integer>(n));

}

NAN_METHOD(PcapWrapper::fetch) {

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
  Local<Object> buffer_obj = args[1]->ToObject();
  u_char *data = (u_char *) node::Buffer::Data(buffer_obj);
  int length = node::Buffer::Length(buffer_obj);
  NanCallback *callback = new NanCallback(args[2].As<Function>());

  wrapper->headers.clear();
  NanAsyncQueueWorker(new Dispatcher(
    callback,
    wrapper->handle,
    batch_size,
    data,
    length,
    wrapper->headers,
    &wrapper->dispatching
  ));

  NanReturnUndefined();

}

NAN_METHOD(PcapWrapper::break_loop) {

  NanScope();
  precondition(args.Length() == 0);
  PcapWrapper *wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());
  check_handle_not_null(wrapper);

  pcap_breakloop(wrapper->handle);

  NanReturnThis();

}

NAN_METHOD(PcapWrapper::close) {

  NanScope();
  precondition(args.Length() == 0);
  PcapWrapper* wrapper = ObjectWrap::Unwrap<PcapWrapper>(args.This());

  pcap_close(wrapper->handle);
  wrapper->handle = NULL;
  if (wrapper->dump_handle != NULL) {
    pcap_dump_close(wrapper->dump_handle);
    wrapper->dump_handle = NULL;
  }
  if (wrapper->on_packet_callback != NULL) {
    delete wrapper->on_packet_callback;
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

void PcapWrapper::on_packet(
  u_char *reader_p,
  const struct pcap_pkthdr *pkthdr,
  const u_char *packet
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
  wrapper->on_packet_callback->Call(3, argv);

  if (try_catch.HasCaught())  {
    node::FatalException(try_catch);
  }

}
