#include "dispatch.hpp"
#include "frame.hpp"
#include "utils.hpp"

namespace Layer2 {

void Init(v8::Handle<v8::Object> exports) {

  Dispatcher::Init(exports);
  Frame::Init(exports);

  // Utilities.

  exports->Set(
    NanNew("getDefaultDevice"),
    NanNew<v8::FunctionTemplate>(GetDefaultDevice)->GetFunction()
  );

  exports->Set(
    NanNew("getLinkInfo"),
    NanNew<v8::FunctionTemplate>(GetLinkInfo)->GetFunction()
  );

  exports->Set(
    NanNew("readMacAddr"),
    NanNew<v8::FunctionTemplate>(ReadMacAddr)->GetFunction()
  );

}

} // Layer2

NODE_MODULE(index, Layer2::Init)
