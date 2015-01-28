#ifndef LAYER2_DOT11_BEACON_PDU_HPP_
#define LAYER2_DOT11_BEACON_PDU_HPP_

#include <nan.h>
#include <tins/tins.h>

class Dot11BeaconPdu : public node::ObjectWrap {

public:
  static void Init();
  static v8::Persistent<v8::FunctionTemplate> constructor;
  Tins::Dot11Beacon *value;

private:
  Dot11BeaconPdu();
  ~Dot11BeaconPdu();
  static NAN_METHOD(New);
  static NAN_METHOD(GetCapabilities);
  static NAN_METHOD(GetSize);

};

#endif
