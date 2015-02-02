#include "dispatch.hpp"
#include "frame.hpp"

#define CHECK(b) if (!(b)) return NanThrowError("Illegal arguments.")

namespace Layer2 {

// Iterator.

v8::Persistent<v8::FunctionTemplate> Iterator::_constructor;

Iterator::Iterator() {

  _index = 0;
  _frames = NULL;

};

Iterator::~Iterator() {

  // We only free data for frames that weren't emitted. The emitted ones are
  // responsible for freeing their individual PDUs.
  if (_frames != NULL) {
    std::vector<struct frame_t *> frames = *_frames;
    while (_index < frames.size()) {
      struct frame_t *data = frames[_index++];
      if (data->pdu != NULL) {
        delete data->pdu;
      }
      delete data;
    }
    delete _frames;
  }

};

NAN_METHOD(Iterator::New) {

  NanScope();
  CHECK(args.Length() == 0);
  Iterator *iterator = new Iterator();
  iterator->Wrap(args.This());
  NanReturnThis();

}

v8::Local<v8::Object> Iterator::NewInstance(int linkType, std::vector<struct frame_t *> *frames) {

  NanEscapableScope();
  v8::Local<v8::FunctionTemplate> constructorHandle = NanNew<v8::FunctionTemplate>(_constructor);
  v8::Local<v8::Object> instance = constructorHandle->GetFunction()->NewInstance();
  Iterator *iterator = Unwrap<Iterator>(instance);
  iterator->_linkType = linkType;
  iterator->_frames = frames;
  return NanEscapeScope(instance);

}

NAN_METHOD(Iterator::Next) {

  NanScope();
  CHECK(args.Length() == 0);
  Iterator *iterator = Unwrap<Iterator>(args.This());

  if (iterator->_index < iterator->_frames->size()) {
    std::vector<struct frame_t *> frames = *iterator->_frames;
    v8::Local<v8::Object> frameInstance = Frame::NewInstance(
      iterator->_linkType,
      frames[iterator->_index++]
    );
    NanReturnValue(frameInstance);
  } else {
    NanReturnNull();
  }

}

void Iterator::Init() {

  NanScope();
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);
  NanAssignPersistent(_constructor, tpl);
  tpl->SetClassName(NanNew("Iterator"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype methods.
  NODE_SET_PROTOTYPE_METHOD(tpl, "next", Next);

  // We don't expose the Iterator constructor.

}

// Parser.

Parser::Parser(
  pcap_t *captureHandle,
  bool *active,
  int batchSize,
  NanCallback *callback
) : NanAsyncWorker(callback) {

  _captureHandle = captureHandle;
  _active = active;
  _linkType = pcap_datalink(_captureHandle);
  _batchSize = batchSize;
  _frames = new std::vector<struct frame_t *>();

}

Parser::~Parser() {};

void Parser::OnPacket(
  u_char *etc,
  const struct pcap_pkthdr *header,
  const u_char *packet
) {

  Parser *parser = (Parser *) etc;
  parser->_frames->push_back(Frame::ParsePdu(parser->_linkType, packet, header));

}

void Parser::Execute() {

  if (*_active) {
    SetErrorMessage("Already dispatching.");
    return;
  }

  *_active = true;
  int n = pcap_dispatch(
    _captureHandle,
    _batchSize,
    OnPacket,
    (u_char *) this
  );

  // Note that we don't propagate the return value of the dispatch call. When
  // reading a file, we can't rely on the output value of it anyway so we
  // always rely on the length of the headers vector instead.

  switch (n) {
    case -1:
      SetErrorMessage(pcap_geterr(_captureHandle));
      break;
    case -2:
      SetErrorMessage("Loop broke.");
      break;
  }

}

void Parser::HandleOKCallback() {

  NanScope();
  *_active = false;
  v8::Local<v8::Value> argv[] = {
    NanNull(), // Error.
    Iterator::NewInstance(_linkType, _frames), // Iterator of frames.
    NanNew<v8::Integer>((int) _frames->size()) // Number of frames in batch.
  };
  callback->Call(3, argv);

}

void Parser::HandleErrorCallback() {

  *_active = false;
  NanAsyncWorker::HandleErrorCallback();

}

// Dispatcher.

v8::Persistent<v8::FunctionTemplate> Dispatcher::_constructor;

Dispatcher::Dispatcher() {

  _captureHandle = NULL;
  _dumpHandle = NULL;
  _device = NULL;
  _dispatching = false;

}

Dispatcher::~Dispatcher() {

  if (_captureHandle != NULL) {
    pcap_close(_captureHandle);
  }
  if (_dumpHandle != NULL) {
    pcap_dump_close(_dumpHandle);
  }

}

// Empty constructor (shouldn't be called from JavaScript).

NAN_METHOD(Dispatcher::New) {

  // Note that we don't set non-blocking as we use a separate thread so better
  // to keep it that way and use a timeout to control the granularity of our
  // dispatch calls.

  NanScope();
  CHECK(args.Length() == 0);
  Dispatcher *dispatcher = new Dispatcher();
  dispatcher->Wrap(args.This());
  NanReturnThis();

}

// Factory methods.

v8::Local<v8::Object> Dispatcher::NewInstance() {

  NanEscapableScope();
  v8::Local<v8::FunctionTemplate> constructorHandle = NanNew<v8::FunctionTemplate>(_constructor);
  v8::Local<v8::Object> instance = constructorHandle->GetFunction()->NewInstance();
  return NanEscapeScope(instance);

}

NAN_METHOD(Dispatcher::FromDevice) {

  NanScope();
  CHECK(
    args.Length() == 6 &&
    args[0]->IsString() && // Interface name.
    args[1]->IsInt32() && // Snapshot length.
    args[2]->IsBoolean() && // Monitor mode.
    args[3]->IsBoolean() && // Promiscuous mode.
    args[4]->IsInt32() && // Timeout.
    args[5]->IsInt32() // Buffer size.
  );
  v8::Local<v8::Object> instance = NewInstance();
  Dispatcher *dispatcher = Unwrap<Dispatcher>(instance);

  // Instantiate capture handle.
  char errbuf[PCAP_ERRBUF_SIZE];
  v8::String::Utf8Value device(args[0]->ToString());
  dispatcher->_device = (char *) *device;
  pcap_t *handle = pcap_create(dispatcher->_device, errbuf);
  if (handle == NULL) {
    return NanThrowError(errbuf);
  }
  dispatcher->_captureHandle = handle;

  // Set all options and activate. These options can only be set before
  // activation (and by extension only also work on live captures).
  if (
    pcap_set_snaplen(dispatcher->_captureHandle, args[1]->Int32Value()) ||
    pcap_set_rfmon(dispatcher->_captureHandle, args[2]->BooleanValue()) ||
    pcap_set_promisc(dispatcher->_captureHandle, args[3]->BooleanValue()) ||
    pcap_set_timeout(dispatcher->_captureHandle, args[4]->Int32Value()) ||
    pcap_set_buffer_size(dispatcher->_captureHandle, args[5]->Int32Value()) ||
    pcap_activate(dispatcher->_captureHandle)
  ) {
    return NanThrowError(pcap_geterr(dispatcher->_captureHandle));
  }

  NanReturnValue(instance);

}

NAN_METHOD(Dispatcher::FromSavefile) {

  NanScope();
  CHECK(
    args.Length() == 1 &&
    args[0]->IsString() // Path to input.
  );
  v8::Local<v8::Object> instance = NewInstance();
  Dispatcher *dispatcher = Unwrap<Dispatcher>(instance);

  char errbuf[PCAP_ERRBUF_SIZE];
  v8::String::Utf8Value path(args[0]->ToString());
  pcap_t *handle = pcap_open_offline((char *) *path, errbuf);
  if (handle == NULL) {
    return NanThrowError(errbuf);
  }
  dispatcher->_captureHandle = handle;

  NanReturnValue(instance);

}

NAN_METHOD(Dispatcher::ToSavefile) {

  // TODO: add write mode option (e.g. append). This would require skipping the
  // header and doing some link type consistency checks.

  NanScope();
  CHECK(
    args.Length() == 3 &&
    args[0]->IsString() && // Path to output.
    args[1]->IsInt32() && // Link type.
    args[2]->IsInt32() // Snapshot length.
  );
  v8::Local<v8::Object> instance = NewInstance();
  Dispatcher *dispatcher = Unwrap<Dispatcher>(instance);

  // Instantiate fake capture handle (this call apparently can't fail, so we
  // don't do error checking here).
  int linktype = args[1]->Int32Value();
  int snaplen = args[2]->Int32Value();
  dispatcher->_captureHandle = pcap_open_dead(linktype, snaplen);

  // Activate dump handle.
  v8::String::Utf8Value path(args[0]->ToString());
  pcap_dumper_t *handle = pcap_dump_open(dispatcher->_captureHandle, (char *) *path);
  if (handle == NULL) {
    return NanThrowError(pcap_geterr(dispatcher->_captureHandle));
  }
  dispatcher->_dumpHandle = handle;

  NanReturnValue(instance);

}

// Configuration and getter methods.

#define EXTRACT() \
  Dispatcher *dispatcher = Unwrap<Dispatcher>(args.This()); \
  pcap_t *handle = dispatcher->_captureHandle; \
  if (handle == NULL) { \
    return NanThrowError("Inactive dispatcher."); \
  }

NAN_METHOD(Dispatcher::SetFilter) {

  NanScope();
  CHECK(args.Length() == 1 && args[0]->IsString());
  EXTRACT();

  int status;
  bpf_u_int32 mask, net;
  char errbuf[PCAP_ERRBUF_SIZE];
  struct bpf_program program;
  v8::String::Utf8Value filter(args[0]->ToString());

  if (filter.length() != 0) {
    if (
      dispatcher->_device == NULL ||
      pcap_lookupnet(dispatcher->_device, &net, &mask, errbuf) == -1
    ) {
      net = PCAP_NETMASK_UNKNOWN;
    }
    if (pcap_compile(handle, &program, (char *) *filter, 1, net) == -1) {
      return NanThrowError(pcap_geterr(handle));
    }
    status = pcap_setfilter(handle, &program);
    pcap_freecode(&program);
    if (status  == -1) {
      return NanThrowError(pcap_geterr(handle));
    }
  }
  NanReturnThis();

}

NAN_METHOD(Dispatcher::GetSnaplen) {

  NanScope();
  CHECK(args.Length() == 0);
  EXTRACT();

  NanReturnValue(NanNew<v8::Integer>(pcap_snapshot(handle)));

}

NAN_METHOD(Dispatcher::GetDatalink) {

  NanScope();
  CHECK(args.Length() == 0);
  EXTRACT();

  NanReturnValue(NanNew<v8::Integer>(pcap_datalink(handle)));

}

NAN_METHOD(Dispatcher::GetStats) {

  NanScope();
  CHECK(args.Length() == 0);
  EXTRACT();

  struct pcap_stat ps;
  if (pcap_stats(handle, &ps) == -1) {
    return NanThrowError(pcap_geterr(handle));
  }
  v8::Local<v8::Object> stats_obj = NanNew<v8::Object>();
  stats_obj->Set(NanNew("psRecv"), NanNew<v8::Integer>(ps.ps_recv));
  stats_obj->Set(NanNew("psDrop"), NanNew<v8::Integer>(ps.ps_drop));
  stats_obj->Set(NanNew("psIfDrop"), NanNew<v8::Integer>(ps.ps_ifdrop));
  NanReturnValue(stats_obj);

}

NAN_METHOD(Dispatcher::Inject) {

  NanScope();
  CHECK(args.Length() == 1 && node::Buffer::HasInstance(args[0]));
  EXTRACT();

  v8::Local<v8::Object> packet = args[0]->ToObject();
  char *packet_data = node::Buffer::Data(packet);
  int packet_length = node::Buffer::Length(packet);

  int n = pcap_inject(handle, packet_data, packet_length);
  if (n == -1) {
    return NanThrowError(pcap_geterr(handle));
  } else if (n != packet_length) {
    return NanThrowError("Frame injection truncated.");
  }

  NanReturnThis();

}

NAN_METHOD(Dispatcher::Dump) { // TODO.

  NanScope();
  // This method should take a Frame object as argument.
  NanReturnThis();

}

NAN_METHOD(Dispatcher::Dispatch) {

  NanScope();
  CHECK(
    args.Length() == 2 &&
    args[0]->IsInt32() && // Batch size.
    args[1]->IsFunction() // Callback.
  );
  EXTRACT();

  int batchSize = args[0]->Int32Value();
  NanCallback *callback = new NanCallback(args[1].As<v8::Function>());
  NanAsyncQueueWorker(new Parser(
    dispatcher->_captureHandle,
    &(dispatcher->_dispatching),
    batchSize,
    callback
  ));

  NanReturnThis();

}

#undef EXTRACT

void Dispatcher::Init(v8::Handle<v8::Object> exports) {

  // Activate iterator.
  Iterator::Init();

  // Create dispatcher constructor template.
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);
  NanAssignPersistent(_constructor, tpl);
  tpl->SetClassName(NanNew("Dispatcher"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype methods.
  NODE_SET_PROTOTYPE_METHOD(tpl, "setFilter", SetFilter);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getSnaplen", GetSnaplen);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getDatalink", GetDatalink);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getStats", GetStats);
  NODE_SET_PROTOTYPE_METHOD(tpl, "inject", Inject);
  NODE_SET_PROTOTYPE_METHOD(tpl, "dump", Dump);
  NODE_SET_PROTOTYPE_METHOD(tpl, "dispatch", Dispatch);

  // Static methods.
  v8::Local<v8::Function> fn = tpl->GetFunction();
  fn->Set(NanNew("fromDevice"), NanNew<v8::FunctionTemplate>(FromDevice)->GetFunction());
  fn->Set(NanNew("fromSavefile"), NanNew<v8::FunctionTemplate>(FromSavefile)->GetFunction());
  fn->Set(NanNew("toSavefile"), NanNew<v8::FunctionTemplate>(ToSavefile)->GetFunction());

  exports->Set(NanNew("Dispatcher"), fn);

}

} // Layer2
