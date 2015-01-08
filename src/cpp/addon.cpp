#include "util.h"
#include "pcap_wrapper.h"


void init(v8::Handle<v8::Object> exports) {

    util_expose(exports);
    pcap_wrapper_expose(exports);

}

NODE_MODULE(index, init)
