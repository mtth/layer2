#include "ethernet_ii.hpp"

EthernetIIPdu::EthernetIIPdu() {}
EthernetIIPdu::~EthernetIIPdu() {}
v8::Persistent<v8::FunctionTemplate> EthernetIIPdu::constructor;

/**
 * Required for function template.
 *
 * Shouldn't however be called directly (as it doesn't set the PDU).
 *
 */
NAN_METHOD(EthernetIIPdu::New) {

  NanScope();

  EthernetIIPdu* pdu = new EthernetIIPdu();
  pdu->Wrap(args.This());
  NanReturnThis();

}

// EthernetII specific methods.

NAN_METHOD(EthernetIIPdu::GetSize) {

  NanScope();

  EthernetIIPdu* pdu = ObjectWrap::Unwrap<EthernetIIPdu>(args.This());
  NanReturnValue(NanNew<v8::Integer>(pdu->value->size()));

}

NAN_METHOD(EthernetIIPdu::GetSrcAddr) {

  NanScope();

  EthernetIIPdu* pdu = ObjectWrap::Unwrap<EthernetIIPdu>(args.This());
  NanReturnValue(NanNew(pdu->value->src_addr().to_string()));

}

NAN_METHOD(EthernetIIPdu::GetDstAddr) {

  NanScope();

  EthernetIIPdu* pdu = ObjectWrap::Unwrap<EthernetIIPdu>(args.This());
  NanReturnValue(NanNew(pdu->value->dst_addr().to_string()));

}

NAN_METHOD(EthernetIIPdu::GetPayloadType) {

  NanScope();

  EthernetIIPdu* pdu = ObjectWrap::Unwrap<EthernetIIPdu>(args.This());
  NanReturnValue(NanNew<v8::Integer>(pdu->value->payload_type()));

}

/**
 * Initializer, setting up the constructor.
 *
 */
void EthernetIIPdu::Init() {

  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);
  NanAssignPersistent(constructor, tpl);
  tpl->SetClassName(NanNew("EthernetIIPdu"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "getSize", GetSize);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getSrcAddr", GetSrcAddr);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getDstAddr", GetDstAddr);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getPayloadType", GetPayloadType);

}
