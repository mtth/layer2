#include "pcap_wrapper.hpp"
#include "util.hpp"
#include "decode/radiotap.hpp"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Local;
using v8::Object;
using v8::String;

void init(v8::Handle<v8::Object> exports) {

  // Pcap wrapper

  char className[] = "PcapWrapper";

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(PcapWrapper::init);
  tpl->SetClassName(NanNew<String>(className));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  #define add_to_prototype(t, s, m) NanSetPrototypeTemplate( \
    (t), \
    (s), \
    NanNew<FunctionTemplate>((m))->GetFunction() \
  )
  add_to_prototype(tpl, "activate", PcapWrapper::activate);
  add_to_prototype(tpl, "breakLoop", PcapWrapper::break_loop);
  add_to_prototype(tpl, "close", PcapWrapper::close);
  add_to_prototype(tpl, "dispatch", PcapWrapper::dispatch);
  add_to_prototype(tpl, "dumpFrame", PcapWrapper::dump);
  add_to_prototype(tpl, "fromDead", PcapWrapper::from_dead);
  add_to_prototype(tpl, "fromDevice", PcapWrapper::from_device);
  add_to_prototype(tpl, "fromSavefile", PcapWrapper::from_savefile);
  add_to_prototype(tpl, "getLinkType", PcapWrapper::get_datalink);
  add_to_prototype(tpl, "getMaxFrameSize", PcapWrapper::get_snaplen);
  add_to_prototype(tpl, "getStats", PcapWrapper::get_stats);
  add_to_prototype(tpl, "injectFrame", PcapWrapper::inject);
  add_to_prototype(tpl, "setBufferSize", PcapWrapper::set_buffersize);
  add_to_prototype(tpl, "setFilter", PcapWrapper::set_filter);
  add_to_prototype(tpl, "setPromisc", PcapWrapper::set_promisc);
  add_to_prototype(tpl, "setMonitor", PcapWrapper::set_rfmon);
  add_to_prototype(tpl, "setMaxFrameSize", PcapWrapper::set_snaplen);
  add_to_prototype(tpl, "toSavefile", PcapWrapper::to_savefile);
  #undef add_to_prototype

  exports->Set(NanNew<String>(className), tpl->GetFunction());

  // Decoders.

  Local<Object> decoders = NanNew<Object>();

  decoders->Set(
    NanNew<String>("IEEE802_11_RADIO"),
    NanNew<FunctionTemplate>(decode_radiotap)->GetFunction()
  );

  exports->Set(
    NanNew<String>("decoders"),
    decoders
  );

  // Utilities.

  exports->Set(
    NanNew<String>("getDefaultDevice"),
    NanNew<FunctionTemplate>(get_default_dev)->GetFunction()
  );

  exports->Set(
    NanNew<String>("readMacAddr"),
    NanNew<FunctionTemplate>(read_mac_addr)->GetFunction()
  );

}

NODE_MODULE(index, init)
