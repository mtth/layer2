#ifndef LAYER2_DECODE_RADIOTAP_HPP_
#define LAYER2_DECODE_RADIOTAP_HPP_

#include <nan.h>
#include <node.h>
#include <tins/tins.h>

/**
 * Class.
 *
 */
class RadioTapFrame : public node::ObjectWrap {

public:

  static void Init();
  static NAN_METHOD(NewInstance);

private:

  explicit RadioTapFrame();
  ~RadioTapFrame();
  static NAN_METHOD(New);
  static NAN_METHOD(GetChannel);

  static v8::Persistent<v8::Function> _constructor;
  Tins::RadioTap *_pdu;

};

#endif
