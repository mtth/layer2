#include "codecs.hpp"

namespace Layer2 {

// Generic.

std::unique_ptr<Layer2::Unsupported> convert(const Tins::PDU &src) {
  Layer2::Unsupported *dst = new Layer2::Unsupported;
  dst->name = Tins::Utils::to_string(src.pdu_type());
  dst->data = const_cast<Tins::PDU &>(src).serialize();
  return std::unique_ptr<Layer2::Unsupported>(dst);
}

// Ethernet II.

std::unique_ptr<Layer2::Ethernet2> convert(const Tins::EthernetII &src) {
  Layer2::Ethernet2 *dst = new Layer2::Ethernet2;

  src.src_addr().copy(dst->srcAddr.data());
  src.dst_addr().copy(dst->dstAddr.data());
  dst->payloadType = src.payload_type();

  Tins::PDU *innerPdu = src.inner_pdu();
  if (innerPdu) {
    dst->data = const_cast<Tins::PDU &>(*innerPdu).serialize();
  }

  return std::unique_ptr<Layer2::Ethernet2>(dst);
}

// 802.11.

// A few helpers first for the nested headers.

void populateDot11Header(Layer2::Dot11Header &dst, const Tins::Dot11 &src) {
  dst.toDs = src.to_ds();
  dst.fromDs = src.from_ds();
  dst.moreFrag = src.more_frag();
  dst.retry = src.retry();
  dst.powerMgmt = src.power_mgmt();
  dst.wep = src.wep();
  dst.order = src.order();
  dst.durationId = src.duration_id();
  src.addr1().copy(dst.addr1.data());
}

void populateDot11MgmtHeader(Layer2::Dot11MgmtHeader &dst, const Tins::Dot11ManagementFrame &src) {
  dst.fragNum = src.frag_num();
  dst.seqNum = src.seq_num();
  src.addr2().copy(dst.addr2.data());
  src.addr3().copy(dst.addr3.data());
  src.addr4().copy(dst.addr4.data());
}

void populateDot11Capabilities(
  std::vector<Layer2::Dot11Capability> &dst,
  const Tins::Dot11ManagementFrame::capability_information &src
) {
  if (src.ess()) {
    dst.push_back(Layer2::Dot11Capability::ESS);
  }
  if (src.ibss()) {
    dst.push_back(Layer2::Dot11Capability::IBSS);
  }
  if (src.cf_poll()) {
    dst.push_back(Layer2::Dot11Capability::CF_POLL);
  }
  if (src.cf_poll_req()) {
    dst.push_back(Layer2::Dot11Capability::CF_POLL_REQ);
  }
  if (src.privacy()) {
    dst.push_back(Layer2::Dot11Capability::PRIVACY);
  }
  if (src.short_preamble()) {
    dst.push_back(Layer2::Dot11Capability::SHORT_PREAMBLE);
  }
  if (src.pbcc()) {
    dst.push_back(Layer2::Dot11Capability::PBCC);
  }
  if (src.channel_agility()) {
    dst.push_back(Layer2::Dot11Capability::CHANNEL_AGILITY);
  }
  if (src.spectrum_mgmt()) {
    dst.push_back(Layer2::Dot11Capability::SPECTRUM_MGMT);
  }
  if (src.qos()) {
    dst.push_back(Layer2::Dot11Capability::QOS);
  }
  if (src.sst()) {
    dst.push_back(Layer2::Dot11Capability::SST);
  }
  if (src.apsd()) {
    dst.push_back(Layer2::Dot11Capability::APSD);
  }
  if (src.radio_measurement()) {
    dst.push_back(Layer2::Dot11Capability::RADIO_MEASUREMENT);
  }
  if (src.dsss_ofdm()) {
    dst.push_back(Layer2::Dot11Capability::DSSS_OFDM);
  }
  if (src.delayed_block_ack()) {
    dst.push_back(Layer2::Dot11Capability::DELAYED_BLOCK_ACK);
  }
  if (src.immediate_block_ack()) {
    dst.push_back(Layer2::Dot11Capability::IMMEDIATE_BLOCK_ACK);
  }
}

// Now the actual PDUs.

std::unique_ptr<Layer2::Dot11AssocRequest> convert(const Tins::Dot11AssocRequest &src) {
  Layer2::Dot11AssocRequest *dst = new Layer2::Dot11AssocRequest;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  dst->listenInterval = src.listen_interval();
  return std::unique_ptr<Layer2::Dot11AssocRequest>(dst);
}

std::unique_ptr<Layer2::Dot11AssocResponse> convert(const Tins::Dot11AssocResponse &src) {
  Layer2::Dot11AssocResponse *dst = new Layer2::Dot11AssocResponse;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  dst->statusCode = src.status_code();
  dst->aid = src.aid();
  return std::unique_ptr<Layer2::Dot11AssocResponse>(dst);
}

std::unique_ptr<Layer2::Dot11Authentication> convert(const Tins::Dot11Authentication &src) {
  Layer2::Dot11Authentication *dst = new Layer2::Dot11Authentication;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  dst->authAlgorithm = src.auth_algorithm();
  dst->authSeqNumber = src.auth_seq_number();
  dst->statusCode = src.status_code();
  return std::unique_ptr<Layer2::Dot11Authentication>(dst);
}

std::unique_ptr<Layer2::Dot11Beacon> convert(const Tins::Dot11Beacon &src) {
  Layer2::Dot11Beacon *dst = new Layer2::Dot11Beacon;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  dst->timestamp = src.timestamp();
  dst->interval = src.interval();
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  return std::unique_ptr<Layer2::Dot11Beacon>(dst);
}

std::unique_ptr<Layer2::Dot11Deauthentication> convert(const Tins::Dot11Deauthentication &src) {
  Layer2::Dot11Deauthentication *dst = new Layer2::Dot11Deauthentication;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  dst->reasonCode = src.reason_code();
  return std::unique_ptr<Layer2::Dot11Deauthentication>(dst);
}

std::unique_ptr<Layer2::Dot11Disassoc> convert(const Tins::Dot11Disassoc &src) {
  Layer2::Dot11Disassoc *dst = new Layer2::Dot11Disassoc;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  dst->reasonCode = src.reason_code();
  return std::unique_ptr<Layer2::Dot11Disassoc>(dst);
}

std::unique_ptr<Layer2::Dot11ProbeRequest> convert(const Tins::Dot11ProbeRequest &src) {
  Layer2::Dot11ProbeRequest *dst = new Layer2::Dot11ProbeRequest;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  return std::unique_ptr<Layer2::Dot11ProbeRequest>(dst);
}

std::unique_ptr<Layer2::Dot11ProbeResponse> convert(const Tins::Dot11ProbeResponse &src) {
  Layer2::Dot11ProbeResponse *dst = new Layer2::Dot11ProbeResponse;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  dst->timestamp = src.timestamp();
  dst->interval = src.interval();
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  return std::unique_ptr<Layer2::Dot11ProbeResponse>(dst);
}

std::unique_ptr<Layer2::Dot11ReassocRequest> convert(const Tins::Dot11ReAssocRequest &src) {
  Layer2::Dot11ReassocRequest *dst = new Layer2::Dot11ReassocRequest;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  dst->listenInterval = src.listen_interval();
  src.current_ap().copy(dst->currentAp.data());
  return std::unique_ptr<Layer2::Dot11ReassocRequest>(dst);
}

std::unique_ptr<Layer2::Dot11ReassocResponse> convert(const Tins::Dot11ReAssocResponse &src) {
  Layer2::Dot11ReassocResponse *dst = new Layer2::Dot11ReassocResponse;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  dst->statusCode = src.status_code();
  dst->aid = src.aid();
  return std::unique_ptr<Layer2::Dot11ReassocResponse>(dst);
}

// Radiotap.

std::unique_ptr<Layer2::Radiotap> convert(const Tins::RadioTap &src) {
  Layer2::Radiotap *dst = new Layer2::Radiotap();

  Tins::RadioTap::PresentFlags present = src.present();
  if (present & Tins::RadioTap::PresentFlags::TSTF) {
    dst->tsft = src.tsft();
  }

  if (present & Tins::RadioTap::PresentFlags::FLAGS) {
    Tins::RadioTap::FrameFlags frameFlags = src.flags();
    if (frameFlags & Tins::RadioTap::FrameFlags::CFP) {
      dst->flags.push_back(Layer2::RadiotapFlag::CFP);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::PREAMBLE) {
      dst->flags.push_back(Layer2::RadiotapFlag::PREAMBLE);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::WEP) {
      dst->flags.push_back(Layer2::RadiotapFlag::WEP);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::FRAGMENTATION) {
      dst->flags.push_back(Layer2::RadiotapFlag::FRAGMENTATION);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::FCS) {
      dst->flags.push_back(Layer2::RadiotapFlag::FCS);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::PADDING) {
      dst->flags.push_back(Layer2::RadiotapFlag::PADDING);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::FAILED_FCS) {
      dst->flags.push_back(Layer2::RadiotapFlag::FAILED_FCS);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::SHORT_GI) {
      dst->flags.push_back(Layer2::RadiotapFlag::SHORT_GI);
    }
  }

  if (present & Tins::RadioTap::PresentFlags::RATE) {
    dst->rate = src.rate();
  }

  if (present & Tins::RadioTap::PresentFlags::CHANNEL) {
    Layer2::RadiotapChannel channel;
    channel.freq = src.channel_freq();
    switch (src.channel_type()) {
    case Tins::RadioTap::TURBO:
      channel.type = Layer2::TURBO;
      break;
    case Tins::RadioTap::CCK:
      channel.type = Layer2::CCK;
      break;
    case Tins::RadioTap::OFDM:
      channel.type = Layer2::OFDM;
      break;
    case Tins::RadioTap::TWO_GZ:
      channel.type = Layer2::TWO_GZ;
      break;
    case Tins::RadioTap::FIVE_GZ:
      channel.type = Layer2::FIVE_GZ;
      break;
    case Tins::RadioTap::PASSIVE:
      channel.type = Layer2::PASSIVE;
      break;
    case Tins::RadioTap::DYN_CCK_OFDM:
      channel.type = Layer2::DYN_CCK_OFDM;
      break;
    case Tins::RadioTap::GFSK:
      channel.type = Layer2::GFSK;
      break;
    }
    dst->channel.set_RadiotapChannel(channel);
  }

  Tins::PDU *innerPdu = src.inner_pdu();
  if (innerPdu) {
    switch (innerPdu->pdu_type()) {
    case Tins::PDU::PDUType::DOT11_ASSOC_REQ:
      dst->pdu.set_Dot11AssocRequest(*Layer2::convert(static_cast<const Tins::Dot11AssocRequest &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_ASSOC_RESP:
      dst->pdu.set_Dot11AssocResponse(*Layer2::convert(static_cast<const Tins::Dot11AssocResponse &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_AUTH:
      dst->pdu.set_Dot11Authentication(*Layer2::convert(static_cast<const Tins::Dot11Authentication &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_BEACON:
      dst->pdu.set_Dot11Beacon(*Layer2::convert(static_cast<const Tins::Dot11Beacon &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_DEAUTH:
      dst->pdu.set_Dot11Deauthentication(*Layer2::convert(static_cast<const Tins::Dot11Deauthentication &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_DIASSOC:
      dst->pdu.set_Dot11Disassoc(*Layer2::convert(static_cast<const Tins::Dot11Disassoc &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_PROBE_REQ:
      dst->pdu.set_Dot11ProbeRequest(*Layer2::convert(static_cast<const Tins::Dot11ProbeRequest &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_PROBE_RESP:
      dst->pdu.set_Dot11ProbeResponse(*Layer2::convert(static_cast<const Tins::Dot11ProbeResponse &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_REASSOC_REQ:
      dst->pdu.set_Dot11ReassocRequest(*Layer2::convert(static_cast<const Tins::Dot11ReAssocRequest &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_REASSOC_RESP:
      dst->pdu.set_Dot11ReassocResponse(*Layer2::convert(static_cast<const Tins::Dot11ReAssocResponse &>(*innerPdu)));
      break;
    default:
      dst->pdu.set_Unsupported(*Layer2::convert(*innerPdu));
    }
  }

  return std::unique_ptr<Layer2::Radiotap>(dst);
}

}
