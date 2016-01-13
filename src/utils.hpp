#pragma once

#include "../etc/deps/avro/lang/c++/api/Stream.hh"
#include <nan.h>

namespace Layer2 {

NAN_METHOD(ReadMacAddr);

class BufferOutputStream : public avro::OutputStream {
public:
  enum State { ALMOST_EMPTY, ALMOST_FULL, FULL };

  static std::unique_ptr<BufferOutputStream> fromBuffer(
    v8::Local<v8::Value> buf,
    float loadFactor
  );

  ~BufferOutputStream() {};

  virtual bool next(uint8_t** data, size_t* len);
  virtual void backup(size_t len);
  virtual void flush();
  virtual uint64_t byteCount() const;
  State getState() const;

private:
  BufferOutputStream(uint8_t *data, size_t len, float loadFactor) :
    _data(data),
    _len(len),
    _hwm(len * loadFactor),
    _pos(0) {};

  uint8_t *_data;
  size_t _len;
  size_t _hwm; // High watermark.
  size_t _pos;
};

} // Layer2
