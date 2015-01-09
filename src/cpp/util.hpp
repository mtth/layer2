#ifndef LAYER2_UTIL_HPP
#define LAYER2_UTIL_HPP

#include <node.h>
#include <pcap/pcap.h>


void util_expose(v8::Handle<v8::Object> exports);

/**
 * Various utilities.
 *
 */
v8::Handle<v8::Value> get_default_dev(const v8::Arguments& args);
v8::Handle<v8::Value> read_mac_addr(const v8::Arguments& args);

#endif
