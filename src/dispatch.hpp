#ifndef LAYER2_DISPATCH_HPP_
#define LAYER2_DISPATCH_HPP_

#include <nan.h>
#include <pcap/pcap.h>
#include <vector>

namespace Layer2 {

/**
 * v8 object passed to the dispatcher's `dispatch` method's callback.
 *
 * Consumers should call `next` on the iterator until it returns `null`
 * (signalling that the iterator is exhausted). The iterator lazily produces
 * the v8 frame objects.
 *
 */
class Iterator : public node::ObjectWrap {

public:
  static void Init();
  static v8::Local<v8::Object> NewInstance(int linktType, std::vector<struct frame_t *> *frames);

private:
  Iterator();
  ~Iterator();
  static NAN_METHOD(New);
  static NAN_METHOD(Next);

  size_t _index;
  int _linkType;
  std::vector<struct frame_t *> *_frames;
  static v8::Persistent<v8::FunctionTemplate> _constructor;

};

/**
 * Asynchronous worker that captures and decodes frames.
 *
 * Once the worker is done, it re-enter the main thread and creates the
 * iterator responsible for feeding the parsed frames back to the VM. It
 * delegates the creation of the actual v8 frame objects to the latter.
 *
 */
class Parser : public NanAsyncWorker {

public:
  Parser(pcap_t *captureHandle, int batchSize, NanCallback *callback);
  ~Parser();
  void Execute();
  void HandleOKCallback();

private:
  static void OnPacket(
    u_char *etc,
    const struct pcap_pkthdr *header,
    const u_char *packet
  );

  pcap_t *_captureHandle;
  int _batchSize;
  int _linkType;
  std::vector<struct frame_t *> *_frames;

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
  static NAN_METHOD(ToSavefile);

  pcap_t *_captureHandle;
  pcap_dumper_t *_dumpHandle;
  char *_device; // For filters.
  static v8::Persistent<v8::FunctionTemplate> _constructor;

};

} // Layer2

#endif
