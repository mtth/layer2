#include "codecs.hpp"
#include "utils.hpp"
#include "wrapper.hpp"

namespace Layer2 {

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
    // First, check whether we have a backlogged PDU.
    std::unique_ptr<Tins::PDU> savedPdu(std::move(_wrapper->_pdu));
    if (savedPdu) {
      avro::encode(*_wrapper->_encoder, *savedPdu);
      switch (_stream->getState()) {
      case BufferOutputStream::State::FULL:
        SetErrorMessage("buffer too small");
      case BufferOutputStream::State::ALMOST_FULL:
        return;
      // Else we have room for more.
      }
    }

    while (true) {
      std::unique_ptr<Tins::PDU> pdu(_wrapper->_sniffer->next_packet());
      if (!pdu) {
        return;
      }
      _numPdus++;
      avro::encode(*_wrapper->_encoder, *pdu);
      switch (_stream->getState()) {
      case BufferOutputStream::State::FULL:
        // There wasn't enough room, we have to save the PDU until the next
        // call (otherwise it will never be transmitted).
        _wrapper->_pdu = std::move(pdu);
        --_numPdus;
      case BufferOutputStream::State::ALMOST_FULL:
        return;
      // Otherwise, continue looping.
      }
    }
  }

  void HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
      Nan::Null(),
      Nan::New<v8::Number>(_numPdus)
    };
    callback->Call(2, argv);
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

NAN_METHOD(Wrapper::FromInterface) {
  if (
    info.Length() < 1 ||
    !info[0]->IsString() ||
    (info.Length() > 1 && !info[1]->IsObject())
  ) {
    Nan::ThrowError("invalid arguments");
    return;
  }

  Nan::Utf8String dev(info[0]);
  Tins::SnifferConfiguration config;
  config.set_promisc_mode(true);
  config.set_rfmon(true);
  Tins::Sniffer *sniffer = new Tins::Sniffer(*dev, config);

  Wrapper *wrapper = new Wrapper(sniffer);
  wrapper->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Wrapper::FromFile) {
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
  Nan::SetPrototypeMethod(tpl, "getPdus", Wrapper::GetPdus);
  Nan::SetPrototypeMethod(tpl, "fromInterface", Wrapper::FromInterface);
  Nan::SetPrototypeMethod(tpl, "fromFile", Wrapper::FromFile);
  return tpl;
}

}
