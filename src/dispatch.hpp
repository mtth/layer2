#pragma once

#include <nan.h>
#include <tins/tins.h>

namespace Layer2 {

/**
 * Asynchronous worker that captures and decodes frames.
 *
 * Once the worker is done, it re-enter the main thread and creates the
 * iterator responsible for feeding the parsed frames back to the VM. It
 * delegates the creation of the actual v8 frame objects to the latter.
 *
 */
class Parser : public Nan::AsyncWorker {

public:
  Parser(Tins::Sniffer sniffer, v8::Local<v8::Value> buf, Nan::Callback *cb);
  ~Parser();
  void Execute();
  void HandleOKCallback();
  void HandleErrorCallback();

private:
  static void OnPdu(Tins::PDU &pdu);

  Tins::Sniffer _sniffer;
  char *_data;
  int _numPdus;
  int _maxBytes;
};

/**
 * v8 object that is the entry point to frame capture.
 *
 * It should be instantiated via its static methods (the constructor will
 * otherwise always yield an inactive capture handle).
 *
 * WARNING: `dispatch` should only ever be have a single call pending. Behavior
 * is undefined if multiple calls are running concurrently.
 *
 */
class Dispatcher : public node::ObjectWrap {

public:
  static void Init(v8::Handle<v8::Object> exports);

private:
  Dispatcher();
  ~Dispatcher();
  static v8::Local<v8::Object> NewInstance();
  static NAN_METHOD(NewInstance);
  static NAN_METHOD(FromDead);
  static NAN_METHOD(FromDevice);
  static NAN_METHOD(FromSavefile);
  static NAN_METHOD(Close);
  static NAN_METHOD(Dump);
  static NAN_METHOD(Dispatch);
  static NAN_METHOD(GetDatalink);
  static NAN_METHOD(GetSnaplen);
  static NAN_METHOD(GetStats);
  static NAN_METHOD(New);
  static NAN_METHOD(Inject);
  static NAN_METHOD(SetFilter);
  static NAN_METHOD(SetSavefile);

  Tins::Sniffer _sniffer;
  static v8::Persistent<v8::FunctionTemplate> _constructor;
};

} // Layer2
