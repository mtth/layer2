#include "codecs.hpp"
#include "wrapper.hpp"
#include <chrono>

namespace Layer2 {

#define LAYER2_BUFFER_SIZE 1024

// Black hole array used to write any overflowing data. We must use this since
// Avro's encoder will throw an error when it tries to write and the underlying
// stream is full. Since this might happen at every loop, it is cheaper to fake
// a successful write rather than catch (which might also have side-effects).
static uint8_t BUFFER[LAYER2_BUFFER_SIZE] = {0};

/**
 * Helper class to handle encoding Avro records to a JavaScript buffer.
 *
 */
class BufferOutputStream : public avro::OutputStream {
public:
  enum State { ALMOST_EMPTY, ALMOST_FULL, FULL };

  static BufferOutputStream *fromBuffer(
    v8::Local<v8::Value> buf,
    float loadFactor
  ) {
    if (!node::Buffer::HasInstance(buf)) {
      return NULL;
    }

    v8::Local<v8::Object> obj = buf->ToObject();
    uint8_t *data = (uint8_t *) node::Buffer::Data(obj);
    size_t len = node::Buffer::Length(obj);
    return new BufferOutputStream(data, len, loadFactor);
  }

  BufferOutputStream(uint8_t *data, size_t len, float loadFactor) :
    _data(data),
    _len(len),
    _hwm(len *loadFactor),
    _pos(0) {};

  ~BufferOutputStream() {};

  virtual bool next(uint8_t **data, size_t *len) {
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

  virtual void backup(size_t len) { _pos -= len; }

  virtual void flush() {}

  virtual uint64_t byteCount() const { return _pos; }

  State getState() const {
    if (_pos < _len) {
      return State::ALMOST_EMPTY;
    }
    if (_pos == _len) {
      return State::ALMOST_FULL;
    }
    return State::FULL;
  }

private:
  uint8_t *_data;
  size_t _len;
  size_t _hwm; // High watermark.
  size_t _pos;
};

/**
 * Helper class to handle asynchronous PDU capture.
 *
 */
class Worker : public Nan::AsyncWorker {
public:
  Worker(Wrapper *wrapper, v8::Local<v8::Value> buf, Nan::Callback *callback) :
  AsyncWorker(callback),
  _wrapper(wrapper),
  _stream(BufferOutputStream::fromBuffer(buf, 0.9)),
  _numPdus(0) {
    _wrapper->_encoder->init(*_stream);
  }

  ~Worker() {}

  void Execute() {
    std::chrono::time_point<std::chrono::high_resolution_clock> start, current;
    start = std::chrono::high_resolution_clock::now();

    // First, check whether we have a backlogged PDU.
    if (_wrapper->_packet.pdu()) {
      avro::encode(*_wrapper->_encoder, _wrapper->_packet);
      delete _wrapper->_packet.release_pdu();
      switch (_stream->getState()) {
      case BufferOutputStream::State::FULL:
        SetErrorMessage("buffer too small");
      case BufferOutputStream::State::ALMOST_FULL:
        return;
      default:
        ; // Else we have room for more.
      }
    }

    while (true) {
      Tins::Packet packet(_wrapper->_sniffer->next_packet());
      if (!packet) {
        return;
      }
      _numPdus++;
      avro::encode(*_wrapper->_encoder, packet);
      switch (_stream->getState()) {
      case BufferOutputStream::State::FULL:
        // There wasn't enough room, we have to save the PDU until the next
        // call (otherwise it will never be transmitted).
        _wrapper->_packet = packet;
        --_numPdus;
      case BufferOutputStream::State::ALMOST_FULL:
        return;
      default:
        // Otherwise, continue looping if the timeout hasn't been reached.
        current = std::chrono::high_resolution_clock::now();
        if (
          _wrapper->_timeout &&
          std::chrono::duration_cast<std::chrono::milliseconds>(current - start).count() > _wrapper->_timeout
        ) {
          return;
        }
      }
    }
  }

  void HandleOKCallback() {
    Nan::HandleScope scope;
    // We have to call this to prevent the encoder from resetting the stream
    // after it has been deleted.
    _wrapper->_encoder->init(*_stream);
    v8::Local<v8::Value> argv[] = {
      Nan::Null(),
      Nan::New<v8::Number>(_numPdus)
    };
    callback->Call(2, argv);
  }

  void HandleErrorCallback() {
    Nan::HandleScope scope;
    _wrapper->_encoder->init(*_stream);
    v8::Local<v8::Value> argv[] = {
      v8::Exception::Error(Nan::New<v8::String>(ErrorMessage()).ToLocalChecked())
    };
    callback->Call(1, argv);
  }

private:
  Wrapper *_wrapper;
  std::unique_ptr<BufferOutputStream> _stream;
  uint32_t _numPdus;
};

// v8 exposed functions.

/**
 * JavaScript constructor.
 *
 * It should always be followed by exactly one call, either to `FromInterface`
 * or to `FromFile`. Behavior otherwise is undefined.
 *
 */
NAN_METHOD(Wrapper::Empty) {}

/**
 * Destructor.
 *
 * This is required to close the capture handle deterministically.
 *
 */
NAN_METHOD(Wrapper::Destroy) {
  Wrapper *wrapper = ObjectWrap::Unwrap<Wrapper>(info.This());
  wrapper->_sniffer.reset();
}

NAN_METHOD(Wrapper::FromInterface) {
  if (
    info.Length() != 7 ||
    !info[0]->IsString() ||
    !(info[1]->IsUndefined() || info[1]->IsUint32()) ||   // snaplen
    !(info[2]->IsUndefined() || info[2]->IsBoolean()) ||  // promisc
    !(info[3]->IsUndefined() || info[3]->IsBoolean()) ||  // rfmon
    !(info[4]->IsUndefined() || info[4]->IsUint32()) ||   // timeout
    !(info[5]->IsUndefined() || info[5]->IsUint32()) ||   // bufferSize
    !(info[6]->IsUndefined() || info[6]->IsString())      // filter
  ) {
    Nan::ThrowError("invalid arguments");
    return;
  }

  // The device to listen on.
  Nan::Utf8String dev(info[0]);

  // Prepare the configuration, only overriding system defaults when a value
  // has been specified.
  Tins::SnifferConfiguration config;
  uint32_t timeout = 0;
  if (!info[1]->IsUndefined()) {
    config.set_snap_len(info[1]->Uint32Value());
  }
  if (!info[2]->IsUndefined()) {
    config.set_promisc_mode(info[2]->BooleanValue());
  }
  if (!info[3]->IsUndefined()) {
    config.set_rfmon(info[3]->BooleanValue());
  }
  if (!info[4]->IsUndefined()) {
    timeout = info[4]->Uint32Value();
    config.set_timeout(timeout);
  }
  if (!info[5]->IsUndefined()) {
    config.set_buffer_size(info[5]->Uint32Value());
  }
  if (!info[6]->IsUndefined()) {
    Nan::Utf8String filter(info[6]);
    config.set_filter(std::string(*filter));
  }

  Tins::Sniffer *sniffer;
  try {
    sniffer = new Tins::Sniffer(*dev, config);
  } catch (std::runtime_error &err) {
    Nan::ThrowError(err.what());
    return;
  }

  Wrapper *wrapper = new Wrapper(sniffer, timeout);
  wrapper->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Wrapper::FromFile) {
  if (
    info.Length() != 2 ||
    !info[0]->IsString() ||
    !(info[1]->IsUndefined() || info[1]->IsString())  // filter
  ) {
    Nan::ThrowError("invalid arguments");
    return;
  }

  // The file to open.
  Nan::Utf8String path(info[0]);

  Tins::SnifferConfiguration config;
  if (!info[1]->IsUndefined()) {
    Nan::Utf8String filter(info[1]);
    config.set_filter(std::string(*filter));
  }

  Tins::FileSniffer *sniffer;
  try {
    sniffer = new Tins::FileSniffer(*path, config);
  } catch (std::runtime_error &err) {
    Nan::ThrowError(err.what());
    return;
  }

  Wrapper *wrapper = new Wrapper(sniffer, 0);
  wrapper->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Wrapper::GetPdus) {
  if (
    info.Length() != 2 ||
    !node::Buffer::HasInstance(info[0]) ||
    !info[1]->IsFunction()
  ) {
    Nan::ThrowError("invalid arguments");
    return;
  }

  Wrapper *wrapper = ObjectWrap::Unwrap<Wrapper>(info.This());
  Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
  Worker *worker = new Worker(wrapper, info[0], callback);
  worker->SaveToPersistent("buffer", info[0]);
  worker->SaveToPersistent("wrapper", info.This());
  Nan::AsyncQueueWorker(worker);
}

/**
 * Initializer, returns the `Wrapper` JavaScript function template.
 *
 */
v8::Local<v8::FunctionTemplate> Wrapper::Init() {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(Wrapper::Empty);
  tpl->SetClassName(Nan::New("Wrapper").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(tpl, "destroy", Wrapper::Destroy);
  Nan::SetPrototypeMethod(tpl, "getPdus", Wrapper::GetPdus);
  Nan::SetPrototypeMethod(tpl, "fromInterface", Wrapper::FromInterface);
  Nan::SetPrototypeMethod(tpl, "fromFile", Wrapper::FromFile);
  return tpl;
}

}
