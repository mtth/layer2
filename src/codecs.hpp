#pragma once

#include "./pdus.hpp"
#include <avro/lang/c++/api/Encoder.hh>
#include <tins/tins.h>

/**
 * Converters between tins PDU data structures and Avro records.
 *
 */

namespace Layer2 {

std::unique_ptr<Layer2::Unsupported> convert(const Tins::PDU &src);
std::unique_ptr<Layer2::Dot11Beacon> convert(const Tins::Dot11Beacon &src);
std::unique_ptr<Layer2::Radiotap> convert(const Tins::RadioTap &src);

}

namespace avro {

template <>
struct codec_traits<Tins::PDU> {

  static void encode(Encoder &encoder, const Tins::PDU &src) {
    Layer2::Pdu dst;
    switch (src.pdu_type()) {
    case Tins::PDU::PDUType::RADIOTAP:
      dst.pdu.set_Radiotap(*Layer2::convert(static_cast<const Tins::RadioTap &>(src)));
      break;
    default:
      dst.pdu.set_Unsupported(*Layer2::convert(src));
    }
    avro::encode(encoder, dst);
  }

};

}
