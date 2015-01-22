#ifndef LAYER2_PCAP_WRAPPER_HPP
#define LAYER2_PCAP_WRAPPER_HPP

#include <nan.h>
#include <node.h>
#include <pcap/pcap.h>
#include <vector>

/**
 * Class for reading packets (either live or from a save file).
 *
 */
class PcapWrapper : public node::ObjectWrap {

public:

  static NAN_METHOD(activate);
  static NAN_METHOD(break_loop);
  static NAN_METHOD(close);
  static NAN_METHOD(dispatch);
  static NAN_METHOD(dump);
  static NAN_METHOD(fetch);
  static NAN_METHOD(from_dead);
  static NAN_METHOD(from_device);
  static NAN_METHOD(from_savefile);
  static NAN_METHOD(get_datalink);
  static NAN_METHOD(get_snaplen);
  static NAN_METHOD(get_stats);
  static NAN_METHOD(init);
  static NAN_METHOD(inject);
  static NAN_METHOD(set_buffersize);
  static NAN_METHOD(set_filter);
  static NAN_METHOD(set_promisc);
  static NAN_METHOD(set_rfmon);
  static NAN_METHOD(set_snaplen);
  static NAN_METHOD(to_savefile);

  void done_dispatching();

private:

  FILE *dump_file_p;
  char *buffer_data;
  char *device;
  pcap_dumper_t *dump_handle;
  pcap_t *handle;
  int buffer_length;
  int buffer_offset;
  bool dispatching;
  std::vector<struct pcap_pkthdr> headers;
  NanCallback *on_packet_callback;

  PcapWrapper();
  ~PcapWrapper();

  static void on_packet(u_char *reader_p, const struct pcap_pkthdr* pkthdr, const u_char* packet);

};

#endif
