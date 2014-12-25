#ifndef NODE_PCAP_WRITER_H
#define NODE_PCAP_WRITER_H

#include <node.h>
#include <pcap/pcap.h>


/**
 * Class for writing packets.
 *
 * It is a convenience to abstract away how the PCAP dumper works (i.e. that it
 * gets generated from a reader first). This lets us consider a writer as a
 * first class citizen.
 *
 */
class PcapWriter : public node::ObjectWrap {

public:

  static void expose(v8::Handle<v8::Object> exports);

private:

  PcapWriter();
  ~PcapWriter();

  static v8::Handle<v8::Value> init(const v8::Arguments& args);
  static v8::Handle<v8::Value> close(const v8::Arguments& args);
  static v8::Handle<v8::Value> fromOptions(const v8::Arguments& args);
  static v8::Handle<v8::Value> fromReader(const v8::Arguments& args);
  static v8::Handle<v8::Value> writePacket(const v8::Arguments& args);

  FILE *file_p;
  pcap_dumper_t *dump_handle;

};

#endif
