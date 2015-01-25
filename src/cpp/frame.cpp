#include "pdu/radiotap.hpp"
#include "frame.hpp"

#define precondition(b) if (!(b)) return NanThrowError("Illegal arguments.")

Frame::Frame(Tins::PDU *pdu) {

  _pdu = pdu;

}

Frame::~Frame() {

  if (_pdu != NULL) {
    delete _pdu;
  }

}

/**
 * Instantiate new frame, returning null if the PDU is invalid.
 *
 * The buffer's contents are copied into the PDU, freeing the buffer for reuse.
 *
 */
NAN_METHOD(Frame::New) {

  NanScope();
  precondition(
    args.Length() == 2 &&
    args[0]->IsInt32() &&
    node::Buffer::HasInstance(args[1])
  );

  // Parse PDU. Note that older versions of libtins (e.g. the one currently
  // installed by Homebrew) don't have the `pdu_from_dlt_flag` method so we
  // replicate it here.
  int linkType = args[0]->Int32Value();
  v8::Local<v8::Object> buffer_obj = args[1]->ToObject();
  const uint8_t *buffer = (const uint8_t *) node::Buffer::Data(buffer_obj);
  uint32_t size = node::Buffer::Length(buffer_obj);
  Tins::PDU *pdu;
  try {
    // Not using `safe_alloc` as defined in libtin's sniffer as it can't be
    // used for 802.11 frames.
    switch (linkType) {
      case DLT_EN10MB:
        pdu = new Tins::EthernetII(buffer, size);
        break;
      case DLT_IEEE802_11_RADIO:
        pdu = new Tins::RadioTap(buffer, size);
        break;
      case DLT_IEEE802_11:
        pdu = Tins::Dot11::from_bytes(buffer, size);
        break;
      case DLT_LINUX_SLL:
        pdu = new Tins::SLL(buffer, size);
        break;
      case DLT_PPI:
        pdu = new Tins::PPI(buffer, size);
        break;
      default:
        return NanThrowError("Unsupported link type.");
    }
  } catch (Tins::malformed_packet&) {
    pdu = NULL;
  }

  Frame *frame = new Frame(pdu);
  frame->Wrap(args.This());
  NanReturnThis();

}

/**
 * Extract PDU of a specific type from the frame, return null if not present.
 *
 * If the PDU type is found but currently not supported, an error will be
 * thrown.
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
  const unsigned argc = 1;
  v8::Handle<v8::Value> argv[argc] = {args[0]};
  v8::Local<v8::FunctionTemplate> constructorHandle;
  v8::Local<v8::Object> pduInstance;
  switch (pdu->pdu_type()) {
    case Tins::PDU::RADIOTAP:
      constructorHandle = NanNew<v8::FunctionTemplate>(RadioTapPdu::constructor);
      pduInstance = constructorHandle->GetFunction()->NewInstance(argc, argv);
      ((RadioTapPdu *) NanGetInternalFieldPointer(pduInstance, 0))->value = static_cast<Tins::RadioTap *>(pdu);
      break;
    default:
      return NanThrowError("Unsupported PDU type.");
  }

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
 * Initialize module.
 *
 * In particular, this function is responsible for initializing the
 * constructors of all PDUs.
 *
 */
void Frame::Init(v8::Handle<v8::Object> exports) {

  // Initialize all PDUs.
  RadioTapPdu::Init();

  // Attach frame, etc.
  char className[] = "Frame";

  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(Frame::New);
  tpl->SetClassName(NanNew(className));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "getPdu", Frame::GetPdu);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getPduTypes", Frame::GetPduTypes);
  NODE_SET_PROTOTYPE_METHOD(tpl, "isValid", Frame::IsValid);

  exports->Set(NanNew(className), tpl->GetFunction());

}
