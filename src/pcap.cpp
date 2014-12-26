#include "pcap_wrapper.h"


void init(v8::Handle<v8::Object> exports) {

    PcapWrapper::expose(exports);

}

NODE_MODULE(pcap, init)
