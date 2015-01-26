#ifndef LAYER2_ETHERNET_II_PDU_HPP_
#define LAYER2_ETHERNET_II_PDU_HPP_

#include <nan.h>
#include <tins/tins.h>

class EthernetIIPdu : public node::ObjectWrap {

public:
  static void Init();
  static v8::Persistent<v8::FunctionTemplate> constructor;
  Tins::EthernetII *value;

private:
  EthernetIIPdu();
  ~EthernetIIPdu();
  static NAN_METHOD(New);
  static NAN_METHOD(GetPayloadType);
  static NAN_METHOD(GetSize);
  static NAN_METHOD(GetSrcAddr);
  static NAN_METHOD(GetDstAddr);

};

#endif
