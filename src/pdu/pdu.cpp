#include "pdu.hpp"

namespace Layer2 {

v8::Persistent<v8::FunctionTemplate> Pdu::constructor;

/**
 * Required for function template.
 *
 * Shouldn't however be called directly (as it doesn't set the PDU).
 *
 */
NAN_METHOD(Pdu::New) {

  NanScope();

  Pdu* pdu = new Pdu();
  pdu->Wrap(args.This());
  NanReturnThis();

}

NAN_METHOD(Pdu::GetSize) {

  NanScope();

  Pdu* pdu = ObjectWrap::Unwrap<Pdu>(args.This());
  NanReturnValue(NanNew<v8::Integer>(pdu->value->size()));

}

/**
 * Initializer, setting up the constructor.
 *
 */
void Pdu::Init() {

  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);
  NanAssignPersistent(constructor, tpl);
  tpl->SetClassName(NanNew("Pdu"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "getSize", GetSize);

}

}
