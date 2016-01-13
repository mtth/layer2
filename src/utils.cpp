#include "utils.hpp"

namespace Layer2 {

#define LAYER2_BUFFER_SIZE 1024

uint8_t BUFFER[LAYER2_BUFFER_SIZE] = {0};

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

// BufferOutputStream

std::unique_ptr<BufferOutputStream> BufferOutputStream::fromBuffer(
  v8::Local<v8::Value> buf,
  float loadFactor
) {
  if (!node::Buffer::HasInstance(buf)) {
    return std::unique_ptr<BufferOutputStream>();
  }

  v8::Local<v8::Object> obj = buf->ToObject();
  uint8_t *data = (uint8_t *) node::Buffer::Data(obj);
  size_t len = node::Buffer::Length(obj);
  return std::unique_ptr<BufferOutputStream>(new BufferOutputStream(data, len, loadFactor));
}

// Can never return false from this function otherwise the encoder will throw
// an error, we must handle this differently.
bool BufferOutputStream::next(uint8_t** data, size_t* len) {
  if (_pos < _hwm) {
    *data = _data + _pos;
    *len = _hwm - _pos;
    _pos = _hwm;
  } else if (_pos < _len) {
    *data = _data + _pos;
    *len = _len - _pos;
    _pos = _len;
  } else {
    *data = BUFFER;
    *len = LAYER2_BUFFER_SIZE;
    _pos = _len + 1;
  }
  return true;
}

void BufferOutputStream::backup(size_t len) { _pos -= len; }

void BufferOutputStream::flush() {}

uint64_t BufferOutputStream::byteCount() const { return _pos; }
BufferOutputStream::State BufferOutputStream::getState() const {
  if (_pos < _len) {
    return State::ALMOST_EMPTY;
  }
  if (_pos == _len) {
    return State::ALMOST_FULL;
  }
  return State::FULL;
};

} // Layer 2
