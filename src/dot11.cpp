#include "pcap.h"


void init(v8::Handle<v8::Object> exports) {

    Pcap::init(exports);

}

NODE_MODULE(dot11, init)
