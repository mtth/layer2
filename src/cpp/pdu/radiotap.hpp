#ifndef LAYER2_RADIOTAP_PDU_HPP_
#define LAYER2_RADIOTAP_PDU_HPP_

#include <nan.h>
#include <tins/tins.h>

class RadioTapPdu : public node::ObjectWrap {

public:
  static void Init();
  static v8::Persistent<v8::Function> constructor;
  Tins::RadioTap *value;

private:
  RadioTapPdu();
  ~RadioTapPdu();
  static NAN_METHOD(New);
  static NAN_METHOD(GetChannel);
  // static NAN_METHOD(GetRate);

};

#endif
