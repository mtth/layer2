#ifndef NODE_PCAP_UTIL_H
#define NODE_PCAP_UTIL_H

#include <node.h>
#include <pcap/pcap.h>


/**
 * Various utilities.
 *
 */

v8::Handle<v8::Value> get_default_dev(const v8::Arguments& args);
v8::Handle<v8::Value> find_all_devs(const v8::Arguments& args);

void util_expose(v8::Handle<v8::Object> exports);

#endif
