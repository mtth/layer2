#ifndef LAYER2_UTIL_HPP
#define LAYER2_UTIL_HPP

#include <nan.h>
#include <node.h>


void util_expose(v8::Handle<v8::Object> exports);

/**
 * Various utilities.
 *
 */
NAN_METHOD(get_default_dev);
NAN_METHOD(read_mac_addr);

#endif
