#pragma once

#include "./pdus.hpp"
#include <avro/Encoder.hh>
#include <tins/tins.h>

/**
 * Converters between tins PDU data structures and Avro records.
 *
 */

namespace Layer2 {

std::unique_ptr<Layer2::ethernet2_Ethernet2> convert(const Tins::EthernetII &src);
std::unique_ptr<Layer2::radiotap_Radiotap> convert(const Tins::RadioTap &src);
std::unique_ptr<Layer2::Unsupported> convert(const Tins::PDU &src);

}

namespace avro {

template <>
struct codec_traits<Tins::Packet> {

  static void encode(Encoder &encoder, const Tins::Packet &src) {
    Layer2::Pdu dst;
    dst.timestamp = src.timestamp().seconds() * 1000 + src.timestamp().microseconds() / 1000;
    const Tins::PDU *pdu = src.pdu();
    if (pdu) {
      dst.size = pdu->size();
      switch (pdu->pdu_type()) {
      case Tins::PDU::PDUType::ETHERNET_II:
        dst.pdu.set_ethernet2_Ethernet2(*Layer2::convert(static_cast<const Tins::EthernetII &>(*pdu)));
        break;
      case Tins::PDU::PDUType::RADIOTAP:
        dst.pdu.set_radiotap_Radiotap(*Layer2::convert(static_cast<const Tins::RadioTap &>(*pdu)));
        break;
      default:
        dst.pdu.set_Unsupported(*Layer2::convert(*pdu));
      }
    } else {
      dst.size = 0;
    }
    avro::encode(encoder, dst);
  }

};

}
