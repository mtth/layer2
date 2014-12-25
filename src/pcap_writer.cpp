#include <node_buffer.h>
#include <node_version.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <cstring>
#include <string.h>
#include "pcap_reader.h"
#include "pcap_writer.h"

#define precondition(b) \
  if (!(b)) \
  return ThrowException(Exception::TypeError(String::New("Illegal arguments.")))
#define add_to_prototype(tpl, s) \
  (tpl)->PrototypeTemplate()->Set( \
    String::NewSymbol(#s), \
    FunctionTemplate::New(s)->GetFunction() \
  )


using namespace v8;

PcapWriter::PcapWriter() {}

PcapWriter::~PcapWriter() {}

// Constructor used from JavaScript.

Handle<Value> PcapWriter::init(const Arguments& args) {

  precondition(args.Length() == 1 && args[0]->IsString());
  HandleScope scope;
  String::Utf8Value path(args[0]->ToString());

  PcapWriter *pcap = new PcapWriter();
  pcap->file_p = fopen((char *) *path, "w");

  pcap->Wrap(args.This());
  return args.This();

}

// Copy settings from reader.

Handle<Value> PcapWriter::fromReader(const Arguments& args) {

  precondition(args.Length() == 1);
  // TODO: check here that argument is of Reader type.
  HandleScope scope;
  PcapWriter* writer = ObjectWrap::Unwrap<PcapWriter>(args.This());

  PcapReader* reader = ObjectWrap::Unwrap<PcapReader>(args[0]->ToObject());
  writer->dump_handle = pcap_dump_fopen(reader->handle, writer->file_p);
  if (writer->dump_handle == NULL) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(reader->handle))));
  }

  return args.This();

}

Handle<Value> PcapWriter::fromOptions(const Arguments& args) {

  precondition(args.Length() == 2 && args[0]->IsString() && args[1]->IsInt32());
  HandleScope scope;
  PcapWriter* writer = ObjectWrap::Unwrap<PcapWriter>(args.This());

  // Extract datalink type.
  String::Utf8Value datalink_name(args[0]->ToString());
  int linktype = pcap_datalink_name_to_val((char *) *datalink_name);
  if (linktype == -1) {
    return ThrowException(Exception::Error(String::New("Invalid datalink.")));
  }

  int snaplen = args[0]->Int32Value();

  pcap_t *handle = pcap_open_dead(linktype, snaplen);
  writer->dump_handle = pcap_dump_fopen(handle, writer->file_p);
  if (writer->dump_handle == NULL) {
    return ThrowException(Exception::Error(String::New(pcap_geterr(handle))));
  }
  pcap_close(handle);

  return args.This();

}

Handle<Value> PcapWriter::close(const Arguments& args) {

  precondition(args.Length() == 0);
  HandleScope scope;
  PcapWriter* pcap = ObjectWrap::Unwrap<PcapWriter>(args.This());

  pcap_dump_close(pcap->dump_handle);
  return Undefined();

}

Handle<Value> PcapWriter::writePacket(const Arguments& args) {

  precondition(args.Length() == 1 && node::Buffer::HasInstance(args[0]));
  HandleScope scope;
  PcapWriter* pcap = ObjectWrap::Unwrap<PcapWriter>(args.This());

  struct timeval tv;
  gettimeofday(&tv, NULL);

  Local<Object> buffer_obj = args[0]->ToObject();
  char *buffer_data = node::Buffer::Data(buffer_obj);
  size_t buffer_length = node::Buffer::Length(buffer_obj);

  // Create fake packet header.
  struct pcap_pkthdr pktheader = {};
  pktheader.ts = tv;
  pktheader.caplen = buffer_length;
  pktheader.len = buffer_length; // WARNING: this is obviously faked.

  pcap_dump((u_char *) pcap->dump_handle, &pktheader, (u_char *) buffer_data);

  return Undefined();

}

// Registration.

void PcapWriter::expose(Handle<Object> exports) {

  char className[] = "Writer";

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(init);
  tpl->SetClassName(String::NewSymbol(className));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  add_to_prototype(tpl, close);
  add_to_prototype(tpl, fromOptions);
  add_to_prototype(tpl, fromReader);
  add_to_prototype(tpl, writePacket);

  exports->Set(
    String::NewSymbol(className),
    Persistent<Function>::New(tpl->GetFunction())
  );

}
