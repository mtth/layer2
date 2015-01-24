#include "radiotap.hpp"
#include <tins/internals.h>

v8::Persistent<v8::Function> RadioTapFrame::_constructor;

RadioTapFrame::RadioTapFrame() {}

RadioTapFrame::~RadioTapFrame() {}

// Static methods.

/**
 * Initializer, setting up the constructor.
 *
 */
void RadioTapFrame::Init() {

  // Create overall template.
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
  tpl->SetClassName(v8::String::NewSymbol("RadioTapFrame"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Assign prototype members.
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("getChannel"),
      v8::FunctionTemplate::New(GetChannel)->GetFunction());

  // Assign to constructor (for use in `NewInstance`).
  _constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());

}

/**
 * Required for function template.
 *
 * Shouldn't however be called directly (as it doesn't set the PDU).
 *
 */
NAN_METHOD(RadioTapFrame::New) {

  NanScope();

  RadioTapFrame* frame = new RadioTapFrame();
  frame->Wrap(args.This());
  return args.This();

}

/**
 * Factory method.
 *
 */
NAN_METHOD(RadioTapFrame::NewInstance) {

  NanScope();

  // Parse PDU.
  v8::Local<v8::Object> buffer_obj = args[0]->ToObject();
  char *buffer = node::Buffer::Data(buffer_obj);
  uint32_t length = node::Buffer::Length(buffer_obj);
  Tins::RadioTap *pdu = new Tins::RadioTap((const uint8_t *) buffer, length);

  // Create wrapper frame object.
  const unsigned argc = 1;
  v8::Handle<v8::Value> argv[argc] = { args[0] };
  v8::Local<v8::Object> instance = _constructor->NewInstance(argc, argv);

  // Attach PDU to frame.
  RadioTapFrame *frame = (RadioTapFrame *) NanGetInternalFieldPointer(instance, 0);
  frame->_pdu = pdu;

  NanReturnValue(instance);

}

NAN_METHOD(RadioTapFrame::GetChannel) {

  NanScope();

  RadioTapFrame* frame = ObjectWrap::Unwrap<RadioTapFrame>(args.This());
  NanReturnValue(NanNew(frame->_pdu->channel_freq()));

}
