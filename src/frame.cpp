#include "pdu/ethernet_ii.hpp"
#include "pdu/radiotap.hpp"
#include "frame.hpp"

#define precondition(b) if (!(b)) return NanThrowError("Illegal arguments.")

namespace Layer2 {

v8::Persistent<v8::FunctionTemplate> Frame::_constructor;

Frame::Frame() {

  _data = NULL;

}

Frame::~Frame() {

  if (_data != NULL) {
    if (_data->pdu != NULL) {
      delete _data->pdu;
    }
    delete _data;
  }

}

/**
 * Instantiate new frame.
 *
 * The buffer's contents are copied into the PDU, freeing the buffer for reuse.
 *
 * This can be called in two different ways.
 *
 */
NAN_METHOD(Frame::New) {

  NanScope();

  Frame *frame;
  switch (args.Length()) {
    case 0: {
      // Probably called from `NewInstance`. Link type, PDU, and packet header
      // will be added directly after the call.
      frame = new Frame();
      break;
    }
    case 2: {
      // Call from JavaScript, we are parsing a buffer.
      precondition(
        args[0]->IsInt32() &&
        node::Buffer::HasInstance(args[1])
      );
      frame = new Frame();
      frame->_linkType = args[0]->Int32Value();
      v8::Local<v8::Object> bufferInstance = args[1]->ToObject();
      size_t size = node::Buffer::Length(bufferInstance);
      pcap_pkthdr header;
      gettimeofday(&header.ts, NULL);
      header.caplen = (bpf_u_int32) size;
      header.len = (bpf_u_int32) size;
      frame->_data = ParsePdu(
        frame->_linkType,
        (u_char *) node::Buffer::Data(bufferInstance),
        &header
      );
      break;
    }
    default:
      return NanThrowError("Invalid arguments.");
  }
  frame->Wrap(args.This());
  NanReturnThis();

}

/**
 * Create new frame.
 *
 * Typically used by dispatch Iterator.
 *
 */
v8::Local<v8::Object> Frame::NewInstance(int linkType, const struct frame_t *data) {

  NanEscapableScope();
  v8::Local<v8::FunctionTemplate> constructorHandle = NanNew<v8::FunctionTemplate>(_constructor);
  v8::Local<v8::Object> instance = constructorHandle->GetFunction()->NewInstance();
  Frame *frame = Unwrap<Frame>(instance);
  frame->_linkType = linkType;
  frame->_data = data;
  return NanEscapeScope(instance);

}

/**
 * Get capture header.
 *
 */
NAN_METHOD(Frame::GetHeader) {

  NanScope();
  precondition(args.Length() == 0);

  Frame* frame = ObjectWrap::Unwrap<Frame>(args.This());
  pcap_pkthdr header = frame->_data->header;

  // Compute time (converting us to ns, similar to `process.hrtime`).
  v8::Handle<v8::Array> tvObj = NanNew<v8::Array>();
  tvObj->Set(0, NanNew<v8::Number>(header.ts.tv_sec));
  tvObj->Set(1, NanNew<v8::Number>(1000 * header.ts.tv_usec));

  v8::Local<v8::Object> headerObj = NanNew<v8::Object>();
  headerObj->Set(NanNew("time"), tvObj);
  headerObj->Set(NanNew("length"), NanNew<v8::Integer>(header.len));
  headerObj->Set(NanNew("capturedLength"), NanNew<v8::Integer>(header.caplen));

  NanReturnValue(headerObj);

}

/**
 * Extract PDU of a specific type from the frame, return null if not present.
 *
 * If the PDU type is found but currently not supported, an error will be
 * thrown.
 *
 * Note that we add the frame back on the PDU to ensure that it won't be
 * garbage collected before the PDU (otherwise methods called on the PDU would
 * have undefined behavior).
 *
 */
NAN_METHOD(Frame::GetPdu) {

  NanScope();
  precondition(
    args.Length() == 1 &&
    args[0]->IsInt32()
  );

  // Extract correct PDU from the frame.
  Frame* frame = ObjectWrap::Unwrap<Frame>(args.This());
  int pduType = args[0]->Int32Value();
  Tins::PDU::PDUType flag = static_cast<Tins::PDU::PDUType>(pduType);
  Tins::PDU *pdu = frame->_data->pdu;
  while (pdu && !pdu->matches_flag(flag)) {
    pdu = pdu->inner_pdu();
  }
  if (pdu == NULL) {
    // No such PDU in this frame (note that invalid flags will fall in this
    // case, we don't do a check if the flag corresponds to an actual
    // implemented PDU libtins type).
    NanReturnNull();
  }

  // Create corresponding JS wrapper instance.

#define INSTANTIATE(P, V) \
  constructorHandle = NanNew<v8::FunctionTemplate>(P::constructor); \
  pduInstance = constructorHandle->GetFunction()->NewInstance(argc, argv); \
  ((P *) NanGetInternalFieldPointer(pduInstance, 0))->value = static_cast<Tins::V *>(pdu); \
  pduInstance->Set(NanNew("_frame"), args.This());

  const unsigned argc = 1;
  v8::Handle<v8::Value> argv[argc] = {args[0]};
  v8::Local<v8::FunctionTemplate> constructorHandle;
  v8::Local<v8::Object> pduInstance;
  switch (pdu->pdu_type()) {
    case Tins::PDU::RADIOTAP:
      INSTANTIATE(RadioTapPdu, RadioTap);
      break;
    case Tins::PDU::ETHERNET_II:
      INSTANTIATE(EthernetIIPdu, EthernetII);
      break;
    default:
      return NanThrowError("Unsupported PDU type.");
  }

#undef INSTANTIATE

  NanReturnValue(pduInstance);

}

/**
 * Get list of all PDU types contained in this frame.
 *
 */
NAN_METHOD(Frame::GetPduTypes) {

  NanScope();
  precondition(args.Length() == 0);

  Frame* frame = ObjectWrap::Unwrap<Frame>(args.This());

  int i = 0;
  v8::Handle<v8::Array> pduTypes = NanNew<v8::Array>();
  Tins::PDU *pdu = frame->_data->pdu;
  while (pdu) {
    pduTypes->Set(i++, NanNew<v8::Integer>(pdu->pdu_type()));
    pdu = pdu->inner_pdu();
  }

  NanReturnValue(pduTypes);

}

/**
 * Check if there is a valid PDU inside the frame.
 *
 */
NAN_METHOD(Frame::IsValid) {

  NanScope();
  precondition(args.Length() == 0);

  Frame* frame = ObjectWrap::Unwrap<Frame>(args.This());
  NanReturnValue(NanNew<v8::Boolean>(frame->_data->isValid));

}

/**
 * Get the size of the entire frame in bytes.
 *
 */
NAN_METHOD(Frame::ToBuffer) {

  NanScope();
  precondition(args.Length() == 0);

  Frame* frame = ObjectWrap::Unwrap<Frame>(args.This());
  Tins::PDU::serialization_type bytes = frame->_data->pdu->serialize();
  NanReturnValue(NanNewBufferHandle((char *) &bytes[0], bytes.size()));
  // Creating a buffer creates a copy of the data.

}

/**
 * Get the size of the entire frame in bytes.
 *
 */
NAN_METHOD(Frame::Size) {

  NanScope();
  precondition(args.Length() == 0);

  Frame* frame = ObjectWrap::Unwrap<Frame>(args.This());
  NanReturnValue(NanNew<v8::Integer>(frame->_data->pdu->size()));

}

/**
 * Extract a PDU.
 *
 */
struct frame_t *Frame::ParsePdu(int linkType, const u_char *bytes, const struct pcap_pkthdr *header) {

  struct frame_t *data = new struct frame_t;
  data->header = *header;

  int size = data->header.caplen;
  try {
    // Not using `safe_alloc` as defined in libtin's sniffer as it can't be
    // used for 802.11 frames.
    switch (linkType) {
      case DLT_EN10MB:
        data->pdu = new Tins::EthernetII(bytes, size);
        break;
      case DLT_IEEE802_11_RADIO:
        data->pdu = new Tins::RadioTap(bytes, size);
        break;
      case DLT_IEEE802_11:
        data->pdu = Tins::Dot11::from_bytes(bytes, size);
        break;
      case DLT_LINUX_SLL:
        data->pdu = new Tins::SLL(bytes, size);
        break;
      case DLT_PPI:
        data->pdu = new Tins::PPI(bytes, size);
        break;
      default:
        data->pdu = new Tins::RawPDU(bytes, size);
    }
    data->isValid = true;
  } catch (Tins::malformed_packet&) {
    data->pdu = new Tins::RawPDU(bytes, size);
    data->isValid = false;
  }

  return data;

}


/**
 * Write PDU. Returns 0 on OK, -1 on error.
 *
 */
int Frame::Dump(pcap_dumper_t *handle) {

  if (handle == NULL) {
    return -1;
  }

  Tins::PDU::serialization_type bytes = _data->pdu->serialize();
  pcap_dump((u_char *) handle, &_data->header, (u_char *) &bytes[0]);
  return 0;

}

/**
 * Check if handle contains a frame.
 *
 */
bool Frame::IsInstance(v8::Handle<v8::Value> handle) {

  return NanHasInstance(_constructor, handle);

}

/**
 * Initialize module.
 *
 * In particular, this function is responsible for initializing the
 * constructors of all PDUs.
 *
 */
void Frame::Init(v8::Handle<v8::Object> exports) {

  // Initialize all PDUs.
  RadioTapPdu::Init();
  EthernetIIPdu::Init();

  // Attach frame, etc.
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(Frame::New);
  NanAssignPersistent(_constructor, tpl);
  tpl->SetClassName(NanNew("Frame"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype methods.
  NODE_SET_PROTOTYPE_METHOD(tpl, "getHeader", GetHeader);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getPdu", GetPdu);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getPduTypes", GetPduTypes);
  NODE_SET_PROTOTYPE_METHOD(tpl, "isValid", IsValid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "toBuffer", ToBuffer);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);

  // Expose constructor.
  exports->Set(NanNew("Frame"), tpl->GetFunction());

}

} // Layer2
