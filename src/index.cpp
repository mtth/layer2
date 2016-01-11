// #include "dispatch.hpp"
#include "utils.hpp"

namespace Layer2 {

void Init(v8::Handle<v8::Object> exports) {

  // Dispatcher::Init(exports);

  // Utilities.

  Nan::Set(
    exports,
    Nan::New<v8::String>("readMacAddr").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(ReadMacAddr)).ToLocalChecked()
  );

}

} // Layer2

NODE_MODULE(index, Layer2::Init)
