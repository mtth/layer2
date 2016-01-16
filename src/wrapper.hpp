#pragma once

#include <nan.h>
#include <tins/tins.h>
#include <avro/Encoder.hh>

namespace Layer2 {

/**
 * Class wrapping tin's Sniffer.
 *
 * It won't be exposed directly from the JavaScript API but rather called from
 * an `EventEmitter` to keep the API simpler (otherwise users would have to
 * deal with decoding the PDUs) and more familiar.
 *
 */
class Wrapper : public Nan::ObjectWrap {
public:

  /**
   * JavaScript glue function.
   *
   * This creates the associated JavaScript function and sets up the prototype,
   * etc.
   *
   */
  static v8::Local<v8::FunctionTemplate> Init();

private:
  friend class Worker;

  std::unique_ptr<Tins::BaseSniffer> _sniffer;
  avro::EncoderPtr _encoder;
  std::unique_ptr<Tins::PDU> _pdu; // Used to store last PDU in case of overflow.
  uint32_t _timeout;

  Wrapper(Tins::BaseSniffer *sniffer, uint32_t timeout) :
  _sniffer(sniffer),
  _timeout(timeout) {
    _encoder = avro::binaryEncoder();
  }

  ~Wrapper() {}

  /**
   * Required function constructor.
   *
   */
  static NAN_METHOD(Empty);

  /**
   * Prototype method which will take in a buffer and a callback.
   *
   * The buffer will be populated with Avro-encoded PDUs. The callback will
   * take in two arguments, an eventual error and the total number of PDUs
   * successfully written to the input buffer.
   *
   */
  static NAN_METHOD(GetPdus);

  /**
   * Factory method to create a `Tins::Sniffer` (live capture).
   *
   */
  static NAN_METHOD(FromInterface);

  /**
   * Factor method to create a `Tins::FileSniffer`.
   *
   */
  static NAN_METHOD(FromFile);
};

}
