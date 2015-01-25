#include "radiotap.hpp"

// TODO: add reference to frame CPP object from pdu CPP object to prevent
// destruction of tins PDU

RadioTapPdu::RadioTapPdu() {}
RadioTapPdu::~RadioTapPdu() {}
v8::Persistent<v8::Function> RadioTapPdu::constructor;

// Static methods.

/**
 * Initializer, setting up the constructor.
 *
 */
void RadioTapPdu::Init() {

  // Create overall template.
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
  tpl->SetClassName(v8::String::NewSymbol("RadioTapPdu"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Assign prototype members.
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("getChannel"),
      v8::FunctionTemplate::New(GetChannel)->GetFunction());

  // Assign to constructor (for use in `NewInstance`).
  constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());

}

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
  return args.This();

}

NAN_METHOD(RadioTapPdu::GetChannel) {

  NanScope();
  RadioTapPdu* pdu = ObjectWrap::Unwrap<RadioTapPdu>(args.This());
  NanReturnValue(NanNew<v8::Integer>(pdu->value->channel_freq()));

}

// #define TINS_METHOD(nodeName, tinsName, type) NAN_METHOD(RadioTapPdu::nodeName) { \
//     NanScope(); \
//     RadioTapPdu* frame = ObjectWrap::Unwrap<RadioTapPdu>(args.This()); \
//     NanReturnValue(NanNew<type>(frame->_pdu->tinsName())); \
//   }
// 
// TINS_METHOD(GetChannel, channel_freq, v8::Integer);
// TINS_METHOD(GetRate, rate, v8::Integer);
