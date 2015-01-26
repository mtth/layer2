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

}

NAN_METHOD(RadioTapPdu::GetDbm) {

  NanScope();

  RadioTapPdu* pdu = ObjectWrap::Unwrap<RadioTapPdu>(args.This());

  v8::Local<v8::Object> dbm = NanNew<v8::Object>();
  dbm->Set(NanNew("signal"), NanNew<v8::Integer>(pdu->value->dbm_signal()));
  dbm->Set(NanNew("noise"), NanNew<v8::Integer>(pdu->value->dbm_noise()));

  NanReturnValue(dbm);

}

NAN_METHOD(RadioTapPdu::GetRate) {

  NanScope();

  RadioTapPdu* pdu = ObjectWrap::Unwrap<RadioTapPdu>(args.This());

  NanReturnValue(NanNew<v8::Integer>(pdu->value->rate()));

}

NAN_METHOD(RadioTapPdu::GetSize) {

  NanScope();

  RadioTapPdu* pdu = ObjectWrap::Unwrap<RadioTapPdu>(args.This());

  NanReturnValue(NanNew<v8::Integer>(pdu->value->size()));

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
  NODE_SET_PROTOTYPE_METHOD(tpl, "getDbm", GetDbm);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getRate", GetRate);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getSize", GetSize);

}
