#include "utils.hpp"

namespace Layer2 {

NAN_METHOD(stringifyAddress) {

  static const char hex[] = "0123456789abcdef";

  if (
    info.Length() != 1 ||
    !node::Buffer::HasInstance(info[0])
  ) {
    Nan::ThrowError("invalid arguments");
    return;
  }

  v8::Local<v8::Object> buf = info[0]->ToObject();
  uint32_t i = node::Buffer::Length(buf);
  if (i == 0) {
    Nan::ThrowError("empty buffer");
    return;
  }

  uint8_t *data = (uint8_t *) node::Buffer::Data(buf);
  uint8_t addr[3 * i];
  uint8_t *cp = addr;

  *cp++ = hex[*data >> 4];
  *cp++ = hex[*data++ & 0x0f];
  while (--i) {
    *cp++ = ':';
    *cp++ = hex[*data >> 4];
    *cp++ = hex[*data++ & 0x0f];
  }
  *cp = '\0';

  info.GetReturnValue().Set(Nan::New<v8::String>((char *) addr).ToLocalChecked());
}

} // Layer 2
