#ifndef NODE_UTIL_H
#define NODE_UTIL_H

#include <node.h>
#include <pcap/pcap.h>


/**
 * Various utilities.
 *
 */

v8::Handle<v8::Value> get_default_dev(const v8::Arguments& args);
v8::Handle<v8::Value> read_mac_addr(const v8::Arguments& args);

void util_expose(v8::Handle<v8::Object> exports);

#endif
