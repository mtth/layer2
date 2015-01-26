#include "radiotap.hpp"

RadioTapPdu::RadioTapPdu() {}
RadioTapPdu::~RadioTapPdu() {}
v8::Persistent<v8::FunctionTemplate> RadioTapPdu::constructor;

/**
 * Required for function template.
 *
 * Shouldn't however be called directly (as it doesn't set the PDU).
 *
 */
NAN_METHOD(RadioTapPdu::New) {

  NanScope();

  RadioTapPdu* pdu = new RadioTapPdu();
  pdu->Wrap(args.This());
  NanReturnThis();

}

// RadioTap specific methods.

NAN_METHOD(RadioTapPdu::GetChannel) {

  NanScope();

  RadioTapPdu* pdu = ObjectWrap::Unwrap<RadioTapPdu>(args.This());

  v8::Local<v8::Object> channel = NanNew<v8::Object>();
  channel->Set(NanNew("freq"), NanNew<v8::Integer>(pdu->value->channel_freq()));
  channel->Set(NanNew("type"), NanNew<v8::Integer>(pdu->value->channel_type()));

  NanReturnValue(channel);
  // TODO: return object with freq, rate, etc.

}

/**
 * Initializer, setting up the constructor.
 *
 */
void RadioTapPdu::Init() {

  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);
  NanAssignPersistent(constructor, tpl);
  tpl->SetClassName(NanNew("RadioTapPdu"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "getChannel", GetChannel);

}
