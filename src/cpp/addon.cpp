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

  NODE_SET_PROTOTYPE_METHOD(tpl, "activate", PcapWrapper::activate);
  NODE_SET_PROTOTYPE_METHOD(tpl, "breakLoop", PcapWrapper::break_loop);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", PcapWrapper::close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "dispatch", PcapWrapper::dispatch);
  NODE_SET_PROTOTYPE_METHOD(tpl, "dumpFrame", PcapWrapper::dump);
  NODE_SET_PROTOTYPE_METHOD(tpl, "fromDead", PcapWrapper::from_dead);
  NODE_SET_PROTOTYPE_METHOD(tpl, "fromDevice", PcapWrapper::from_device);
  NODE_SET_PROTOTYPE_METHOD(tpl, "fromSavefile", PcapWrapper::from_savefile);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getLinkType", PcapWrapper::get_datalink);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getMaxFrameSize", PcapWrapper::get_snaplen);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getStats", PcapWrapper::get_stats);
  NODE_SET_PROTOTYPE_METHOD(tpl, "injectFrame", PcapWrapper::inject);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setBufferSize", PcapWrapper::set_buffersize);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setFilter", PcapWrapper::set_filter);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setPromisc", PcapWrapper::set_promisc);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setMonitor", PcapWrapper::set_rfmon);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setMaxFrameSize", PcapWrapper::set_snaplen);
  NODE_SET_PROTOTYPE_METHOD(tpl, "toSavefile", PcapWrapper::to_savefile);

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
