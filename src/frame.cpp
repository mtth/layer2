#include "pdu/ethernet_ii.hpp"
#include "pdu/radiotap.hpp"
#include "frame.hpp"

#define precondition(b) if (!(b)) return NanThrowError("Illegal arguments.")

namespace Layer2 {

v8::Persistent<v8::FunctionTemplate> Frame::_constructor;

Frame::Frame() {

  _pdu = NULL;

}

Frame::~Frame() {

  if (_pdu != NULL) {
    delete _pdu;
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
      // TODO: create fake packet header data here.
      precondition(
        args[0]->IsInt32() &&
        node::Buffer::HasInstance(args[1])
      );
      v8::Local<v8::Object> bufferInstance = args[1]->ToObject();
      frame = new Frame();
      frame->_linkType = args[0]->Int32Value();
      frame->_pdu = ParsePdu(
        frame->_linkType,
        (u_char *) node::Buffer::Data(bufferInstance),
        node::Buffer::Length(bufferInstance)
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
v8::Local<v8::Object> Frame::NewInstance(int linkType, Tins::PDU *pdu) {

  NanEscapableScope();
  v8::Local<v8::FunctionTemplate> constructorHandle = NanNew<v8::FunctionTemplate>(_constructor);
  v8::Local<v8::Object> instance = constructorHandle->GetFunction()->NewInstance();
  Frame *frame = Unwrap<Frame>(instance);
  frame->_linkType = linkType;
  frame->_pdu = pdu;
  // TODO: populate packet header on frame.
  return NanEscapeScope(instance);

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
  Tins::PDU *pdu = frame->_pdu;
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
  Tins::PDU *pdu = frame->_pdu;
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
  NanReturnValue(NanNew<v8::Boolean>(frame->_pdu != NULL));

}

/**
 * Extract a PDU.
 *
 */
Tins::PDU *Frame::ParsePdu(int linkType, const u_char *bytes, int size) {

  Tins::PDU *pdu;
  try {
    // Not using `safe_alloc` as defined in libtin's sniffer as it can't be
    // used for 802.11 frames.
    switch (linkType) {
      case DLT_EN10MB:
        pdu = new Tins::EthernetII(bytes, size);
        break;
      case DLT_IEEE802_11_RADIO:
        pdu = new Tins::RadioTap(bytes, size);
        break;
      case DLT_IEEE802_11:
        pdu = Tins::Dot11::from_bytes(bytes, size);
        break;
      case DLT_LINUX_SLL:
        pdu = new Tins::SLL(bytes, size);
        break;
      case DLT_PPI:
        pdu = new Tins::PPI(bytes, size);
        break;
      default:
        pdu = new Tins::RawPDU(bytes, size);
    }
  } catch (Tins::malformed_packet&) {
    pdu = NULL;
  }

  return pdu;

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
  NODE_SET_PROTOTYPE_METHOD(tpl, "getPdu", Frame::GetPdu);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getPduTypes", Frame::GetPduTypes);
  NODE_SET_PROTOTYPE_METHOD(tpl, "isValid", Frame::IsValid);

  // Expose constructor.
  exports->Set(NanNew("Frame"), tpl->GetFunction());

}

} // Layer2
