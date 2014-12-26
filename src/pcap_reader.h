#ifndef NODE_PCAP_READER_H
#define NODE_PCAP_READER_H

#include <node.h>
#include <pcap/pcap.h>


class PcapWriter;

/**
 * Class for reading packets (either live or from a save file).
 *
 */
class PcapReader : public node::ObjectWrap {

public:

  static void expose(v8::Handle<v8::Object> exports);

private:

  friend class PcapWriter;

  v8::Persistent<v8::Function> on_packet_callback;
  struct bpf_program filter;
  pcap_t *handle;
  char *buffer_data;
  size_t buffer_length;
  size_t buffer_offset;

  PcapReader();
  ~PcapReader();

  static v8::Handle<v8::Value> activate(const v8::Arguments& args);
  static v8::Handle<v8::Value> break_loop(const v8::Arguments& args);
  static v8::Handle<v8::Value> close(const v8::Arguments& args);
  static v8::Handle<v8::Value> dispatch(const v8::Arguments& args);
  static v8::Handle<v8::Value> from_device(const v8::Arguments& args);
  static v8::Handle<v8::Value> from_savefile(const v8::Arguments& args);
  static v8::Handle<v8::Value> get_datalink(const v8::Arguments& args);
  static v8::Handle<v8::Value> get_snaplen(const v8::Arguments& args);
  static v8::Handle<v8::Value> get_stats(const v8::Arguments& args);
  static v8::Handle<v8::Value> init(const v8::Arguments& args);
  static v8::Handle<v8::Value> inject(const v8::Arguments& args);
  static v8::Handle<v8::Value> set_buffersize(const v8::Arguments& args);
  static v8::Handle<v8::Value> set_filter(const v8::Arguments& args);
  static v8::Handle<v8::Value> set_promisc(const v8::Arguments& args);
  static v8::Handle<v8::Value> set_rfmon(const v8::Arguments& args);
  static v8::Handle<v8::Value> set_snaplen(const v8::Arguments& args);
  static void on_packet(u_char *reader_p, const struct pcap_pkthdr* pkthdr, const u_char* packet);

};

#endif
