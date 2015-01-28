#ifndef LAYER2_FRAME_HPP_
#define LAYER2_FRAME_HPP_

#include <nan.h>
#include <tins/tins.h>

namespace Layer2 {

class Frame : public node::ObjectWrap {

public:
  static void Init(v8::Handle<v8::Object> exports);
  static v8::Local<v8::Object> NewInstance(int linkType, Tins::PDU *pdu);
  static Tins::PDU *ParsePdu(int linkType, const u_char *bytes, int length);

private:
  Frame();
  ~Frame();
  static NAN_METHOD(New);
  static NAN_METHOD(GetPduTypes);
  static NAN_METHOD(GetPdu);
  static NAN_METHOD(IsValid);

  int _linkType;
  struct pcap_pkthdr _header;
  Tins::PDU *_pdu;
  static v8::Persistent<v8::FunctionTemplate> _constructor;

};

} // Layer2

#endif
