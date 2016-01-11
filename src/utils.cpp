#include "utils.hpp"

namespace Layer2 {

NAN_METHOD(ReadMacAddr) {
  static const char hex[] = "0123456789abcdef";

  if (
    info.Length() != 2 ||
    !node::Buffer::HasInstance(info[0]) ||
    !info[1]->IsInt32()
  ) {
    return Nan::ThrowError("Illegal arguments.");
  }

  v8::Local<v8::Object> buf = info[0]->ToObject();
  int offset = info[1]->Int32Value();
  if (offset < 0 || (size_t) (offset + 6) > node::Buffer::Length(buf)) {
    return Nan::ThrowError("Invalid offset");
  }

  int i = 5;
  unsigned char *data = (unsigned char *) node::Buffer::Data(buf) + offset;
  char addr[18];
  char *cp = addr;

  *cp++ = hex[*data >> 4];
  *cp++ = hex[*data++ & 0x0f];
  while (i--) {
    *cp++ = ':';
    *cp++ = hex[*data >> 4];
    *cp++ = hex[*data++ & 0x0f];
  }
  *cp = '\0';

  info.GetReturnValue().Set(Nan::New<v8::String>(addr).ToLocalChecked());
}

} // Layer 2
