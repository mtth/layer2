#ifndef LAYER2_FRAME_HPP_
#define LAYER2_FRAME_HPP_

#include <nan.h>
#include <tins/tins.h>

class Frame : public node::ObjectWrap {

public:
  static void Init(v8::Handle<v8::Object> exports);

private:
  Frame(Tins::PDU *pdu);
  ~Frame();
  static NAN_METHOD(New);
  static NAN_METHOD(GetPduTypes);
  static NAN_METHOD(GetPdu);
  static NAN_METHOD(IsValid);
  Tins::PDU *_pdu;
  static v8::Persistent<v8::Function> _constructor;

};

#endif
