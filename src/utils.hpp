#ifndef LAYER2_UTILS_HPP_
#define LAYER2_UTILS_HPP_

#include <nan.h>

namespace Layer2 {

NAN_METHOD(GetDefaultDevice);
NAN_METHOD(GetLinkInfo);
NAN_METHOD(ReadMacAddr);

} // Layer2

#endif
