#include "pdu/radiotap.hpp"
#include "frame.hpp"

Frame::Frame(Tins::PDU *pdu) {

  _pdu = pdu;

}

Frame::~Frame() {

  delete _pdu;

}

NAN_METHOD(Frame::New) {

  NanScope();

  // Parse PDU.
  int flag = args[0]->Int32Value();
  v8::Local<v8::Object> buffer_obj = args[1]->ToObject();
  const uint8_t *buffer = (const uint8_t *) node::Buffer::Data(buffer_obj);
  uint32_t size = node::Buffer::Length(buffer_obj);

  // Older versions of libtins don't have the `pdu_from_dlt_flag` method.
  // TODO: do version check and call function if available.
  Tins::PDU *pdu;
  switch (flag) {
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
      pdu = NULL;
  }

  if (pdu != NULL) {
    Frame *frame = new Frame(pdu);
    frame->Wrap(args.This());
    NanReturnThis();
  } else {
    NanReturnNull();
  }

}

NAN_METHOD(Frame::GetPdu) {

  NanScope();
  Frame* frame = ObjectWrap::Unwrap<Frame>(args.This());

  int type = args[0]->Int32Value();
  Tins::PDU::PDUType pduType = static_cast<Tins::PDU::PDUType>(type);
  Tins::PDU *pdu = frame->_pdu;
  while (pdu && !pdu->matches_flag(pduType)) {
    pdu = pdu->inner_pdu();
  }
  if (pdu == NULL) {
    // No such PDU in this frame.
    NanReturnNull();
  }

  // Create corresponding JS wrapper instance.
  const unsigned argc = 1;
  v8::Handle<v8::Value> argv[argc] = {args[0]};
  v8::Local<v8::Object> pduInstance;
  switch (pdu->pdu_type()) { // switch on pdu type.
  case Tins::PDU::RADIOTAP:
    pduInstance = RadioTapPdu::constructor->NewInstance(argc, argv);
    ((RadioTapPdu *) NanGetInternalFieldPointer(pduInstance, 0))->value = static_cast<Tins::RadioTap *>(pdu);
    break;
  default:
    return NanThrowError("Unsupported PDU.");
  }

  NanReturnValue(pduInstance);

}

NAN_METHOD(Frame::GetPduTypes) {

  NanReturnNull();

}

void Frame::Init(v8::Handle<v8::Object> exports) {

  // Initialize all PDUs.
  RadioTapPdu::Init();

  // Attach frame, etc.
  char className[] = "Frame";

  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(Frame::New);
  tpl->SetClassName(NanNew(className));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "getPdu", Frame::GetPdu);

  exports->Set(NanNew(className), tpl->GetFunction());

}
