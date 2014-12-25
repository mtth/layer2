#include "pcap_reader.h"
#include "pcap_writer.h"


void init(v8::Handle<v8::Object> exports) {

    PcapReader::expose(exports);
    PcapWriter::expose(exports);

}

NODE_MODULE(pcap, init)
