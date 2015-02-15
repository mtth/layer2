#include "dot11_beacon.hpp"

Dot11BeaconPdu::Dot11BeaconPdu() {}
Dot11BeaconPdu::~Dot11BeaconPdu() {}
v8::Persistent<v8::FunctionTemplate> Dot11BeaconPdu::constructor;

/**
 * Required for function template.
 *
 * Shouldn't however be called directly (as it doesn't set the PDU).
 *
 */
NAN_METHOD(Dot11BeaconPdu::New) {

  NanScope();

  Dot11BeaconPdu* pdu = new Dot11BeaconPdu();
  pdu->Wrap(args.This());
  NanReturnThis();

}

// Dot11Beacon specific methods.

NAN_METHOD(Dot11BeaconPdu::GetSize) {

  NanScope();

  Dot11BeaconPdu* pdu = ObjectWrap::Unwrap<Dot11BeaconPdu>(args.This());
  NanReturnValue(NanNew<v8::Integer>(pdu->value->size()));

}

NAN_METHOD(Dot11BeaconPdu::GetAddresses) {

  NanScope();

  Dot11BeaconPdu* pdu = ObjectWrap::Unwrap<Dot11BeaconPdu>(args.This());
  v8::Handle<v8::Array> addresses = NanNew<v8::Array>();
  addresses->Set(0, NanNew<v8::String>(pdu->value->addr1()));
  addresses->Set(1, NanNew<v8::String>(pdu->value->addr2()));
  addresses->Set(2, NanNew<v8::String>(pdu->value->addr3()));
  addresses->Set(3, NanNew<v8::String>(pdu->value->addr4()));

  NanReturnValue(addresses);

}

/**
 * Initializer, setting up the constructor.
 *
 */
void Dot11BeaconPdu::Init() {

  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);
  NanAssignPersistent(constructor, tpl);
  tpl->SetClassName(NanNew("Dot11BeaconPdu"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "getSize", GetSize);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getAddresses", GetAddresses);

}
