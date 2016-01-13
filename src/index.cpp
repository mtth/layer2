// #include "dispatch.hpp"
#include "wrapper.hpp"
#include "utils.hpp"

namespace Layer2 {

// NAN_METHOD(Test) {
//   if (info.Length() != 1) {
//     return Nan::ThrowError("Illegal arguments.");
//   }
// 
//   Layer2::BufferOutputStream *out = BufferOutputStream::fromBuffer(info[0], 0.9);
//   if (!out) {
//     return Nan::ThrowError("Not a buffer.");
//   }
//   avro::EncoderPtr e = avro::binaryEncoder();
//   e->init(*out);
// 
//   int n = 0;
//   // Tins::Sniffer sniffer("en0");
//   Tins::Sniffer sniffer("en0", Tins::Sniffer::promisc_type::PROMISC, "", true);
//   while (out->getState() == ) {
//     Tins::PDU *_pdu = sniffer.next_packet();
//     std::cout << "got a packet " << _pdu << " type: " << _pdu->pdu_type();
//     std::cout << "with rate" << _pdu->" \n";
//     avro::encode(*e, *_pdu);
//     n++;
//     // if (_pdu) {
//     //   Tins::RadioTap *pdu = _pdu->find_pdu<Tins::RadioTap>();
//     //   if (pdu) {
//     //   } else {
//     //     avro::encode(*e, *_pdu);
//     //   }
//     // }
//   }
//   if (out->getState() == Layer2::BufferOutputStream::State::FULL) {
//     // We might have lost the last one.
//     n--;
//   }
// 
// 
//   // while (out->getState() == Layer2::BufferOutputStream::State::EMPTY) {
//   //   Tins::RadioTap pdu;
//   //   pdu.rate(n);
//   //   n++;
//   //   avro::encode(*e, pdu);
//   // }
// 
//   info.GetReturnValue().Set(Nan::New<v8::Integer>(n));
// }

void Init(v8::Handle<v8::Object> exports) {

  Nan::Set(
    exports,
    Nan::New<v8::String>("Wrapper").ToLocalChecked(),
    Nan::GetFunction(Wrapper::Init()).ToLocalChecked()
  );

  // Utilities.

  Nan::Set(
    exports,
    Nan::New<v8::String>("readMacAddr").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(ReadMacAddr)).ToLocalChecked()
  );

  // Nan::Set(
  //   exports,
  //   Nan::New<v8::String>("test").ToLocalChecked(),
  //   Nan::GetFunction(Nan::New<v8::FunctionTemplate>(Test)).ToLocalChecked()
  // );

}

} // Layer2

NODE_MODULE(index, Layer2::Init)
