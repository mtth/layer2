#ifndef LAYER2_PCAP_WRAPPER_HPP
#define LAYER2_PCAP_WRAPPER_HPP

#include <node.h>
#include <pcap/pcap.h>


void pcap_wrapper_expose(v8::Handle<v8::Object> exports);

/**
 * Class for reading packets (either live or from a save file).
 *
 */
class PcapWrapper : public node::ObjectWrap {

private:

  friend void pcap_wrapper_expose(v8::Handle<v8::Object> exports);

  FILE *dump_file_p;
  char *buffer_data;
  pcap_dumper_t *dump_handle;
  pcap_t *handle;
  size_t buffer_length;
  size_t buffer_offset;
  struct bpf_program filter;
  v8::Persistent<v8::Function> on_packet_callback;

  PcapWrapper();
  ~PcapWrapper();

  static v8::Handle<v8::Value> activate(const v8::Arguments& args);
  static v8::Handle<v8::Value> break_loop(const v8::Arguments& args);
  static v8::Handle<v8::Value> close(const v8::Arguments& args);
  static v8::Handle<v8::Value> dispatch(const v8::Arguments& args);
  static v8::Handle<v8::Value> dump(const v8::Arguments& args);
  static v8::Handle<v8::Value> from_dead(const v8::Arguments& args);
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
  static v8::Handle<v8::Value> to_savefile(const v8::Arguments& args);
  static void on_packet(u_char *reader_p, const struct pcap_pkthdr* pkthdr, const u_char* packet);

};

#endif
