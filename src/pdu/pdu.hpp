#ifndef LAYER2_PDU_HPP_
#define LAYER2_PDU_HPP_

// Base PDU class.

#include <nan.h>
#include <tins/tins.h>

namespace Layer2 {

class Pdu : public node::ObjectWrap {

public:
  static void Init();
  static v8::Persistent<v8::FunctionTemplate> constructor;
  Tins::PDU *value;

private:
  static NAN_METHOD(New);
  static NAN_METHOD(GetSize);

};

}

#endif
