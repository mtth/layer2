#include "wrapper.hpp"

namespace Layer2 {

void Init(v8::Handle<v8::Object> exports) {

  Nan::Set(
    exports,
    Nan::New<v8::String>("Wrapper").ToLocalChecked(),
    Nan::GetFunction(Wrapper::Init()).ToLocalChecked()
  );

}

} // Layer2

NODE_MODULE(index, Layer2::Init)
