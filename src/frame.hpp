#ifndef LAYER2_FRAME_HPP_
#define LAYER2_FRAME_HPP_

#include <nan.h>
#include <tins/tins.h>

namespace Layer2 {

/**
 * Frame data.
 *
 */
struct frame_t {
  struct pcap_pkthdr header;
  Tins::PDU *pdu;
  bool isValid;
};

/**
 * v8 object wrapping a captured frame.
 *
 * In most cases (i.e. unless the link type isn't supported), the frame will
 * also be decoded.
 *
 */
class Frame : public node::ObjectWrap {

public:
  int Dump(pcap_dumper_t *handle);
  static void Init(v8::Handle<v8::Object> exports);
  static v8::Local<v8::Object> NewInstance(int linkType, const struct frame_t *data);
  static bool IsInstance(v8::Handle<v8::Value> val);
  static struct frame_t *ParsePdu(int linkType, const u_char *bytes, const struct pcap_pkthdr *header);

private:
  Frame();
  ~Frame();
  static NAN_METHOD(New);
  static NAN_METHOD(GetData);
  static NAN_METHOD(GetHeader);
  static NAN_METHOD(GetLinkType); // TODO.
  static NAN_METHOD(GetPduTypes);
  static NAN_METHOD(GetPdu);
  static NAN_METHOD(IsValid);
  static NAN_METHOD(Size);

  int _linkType;
  const struct frame_t *_data;
  static v8::Persistent<v8::FunctionTemplate> _constructor;

};

} // Layer2

#endif
