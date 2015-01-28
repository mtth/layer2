#ifndef LAYER2_DISPATCH_HPP_
#define LAYER2_DISPATCH_HPP_

#include <nan.h>
#include <pcap/pcap.h>
#include <vector>
#include <tins/tins.h>

namespace Layer2 {

class Iterator : public node::ObjectWrap {

public:
  static void Init();
  static v8::Local<v8::Object> NewInstance(int linktType, std::vector<Tins::PDU *> *pdus);

private:
  Iterator();
  ~Iterator();
  static NAN_METHOD(New);
  static NAN_METHOD(Next);

  size_t _index;
  int _linkType;
  std::vector<Tins::PDU *> *_pdus;
  static v8::Persistent<v8::FunctionTemplate> _constructor;

};

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
  std::vector<Tins::PDU *> *_pdus;

};

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
  bool _isDispatching; // Safety checks.
  static v8::Persistent<v8::FunctionTemplate> _constructor;

};

} // Layer2

#endif
