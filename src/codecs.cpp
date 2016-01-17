#include "codecs.hpp"

namespace Layer2 {

// Generic.

std::unique_ptr<Layer2::Unsupported> convert(const Tins::PDU &src) {
  Layer2::Unsupported *dst = new Layer2::Unsupported;
  dst->name = Tins::Utils::to_string(src.pdu_type());
  return std::unique_ptr<Layer2::Unsupported>(dst);
}

// Ethernet II.

std::unique_ptr<Layer2::ethernet2_Ethernet2> convert(const Tins::EthernetII &src) {
  Layer2::ethernet2_Ethernet2 *dst = new Layer2::ethernet2_Ethernet2;

  src.src_addr().copy(dst->srcAddr.data());
  src.dst_addr().copy(dst->dstAddr.data());
  dst->payloadType = src.payload_type();

  Tins::PDU *innerPdu = src.inner_pdu();
  if (innerPdu) {
    dst->data = const_cast<Tins::PDU &>(*innerPdu).serialize();
  }

  return std::unique_ptr<Layer2::ethernet2_Ethernet2>(dst);
}

// 802.11.

// A few helpers first for the nested headers.

void populateDot11Header(Layer2::dot11_Header &dst, const Tins::Dot11 &src) {
  dst.type = src.type();
  dst.subtype = src.subtype();
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

void populateDot11DataHeader(Layer2::dot11_data_Header &dst, const Tins::Dot11Data &src) {
  dst.fragNum = src.frag_num();
  dst.seqNum = src.seq_num();
  src.addr2().copy(dst.addr2.data());
  src.addr3().copy(dst.addr3.data());
  src.addr4().copy(dst.addr4.data());
}

void populateDot11MgmtHeader(Layer2::dot11_mgmt_Header &dst, const Tins::Dot11ManagementFrame &src) {
  dst.fragNum = src.frag_num();
  dst.seqNum = src.seq_num();
  src.addr2().copy(dst.addr2.data());
  src.addr3().copy(dst.addr3.data());
  src.addr4().copy(dst.addr4.data());
}

void populateDot11Capabilities(
  std::vector<Layer2::dot11_mgmt_Capability> &dst,
  const Tins::Dot11ManagementFrame::capability_information &src
) {
  if (src.ess()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::ESS);
  }
  if (src.ibss()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::IBSS);
  }
  if (src.cf_poll()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::CF_POLL);
  }
  if (src.cf_poll_req()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::CF_POLL_REQ);
  }
  if (src.privacy()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::PRIVACY);
  }
  if (src.short_preamble()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::SHORT_PREAMBLE);
  }
  if (src.pbcc()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::PBCC);
  }
  if (src.channel_agility()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::CHANNEL_AGILITY);
  }
  if (src.spectrum_mgmt()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::SPECTRUM_MGMT);
  }
  if (src.qos()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::QOS);
  }
  if (src.sst()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::SST);
  }
  if (src.apsd()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::APSD);
  }
  if (src.radio_measurement()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::RADIO_MEASUREMENT);
  }
  if (src.dsss_ofdm()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::DSSS_OFDM);
  }
  if (src.delayed_block_ack()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::DELAYED_BLOCK_ACK);
  }
  if (src.immediate_block_ack()) {
    dst.push_back(Layer2::dot11_mgmt_Capability::IMMEDIATE_BLOCK_ACK);
  }
}

// Now the actual PDUs.

std::unique_ptr<Layer2::dot11_ctrl_Ack> convert(const Tins::Dot11Ack &src) {
  Layer2::dot11_ctrl_Ack *dst = new Layer2::dot11_ctrl_Ack;
  populateDot11Header(dst->header, src);
  return std::unique_ptr<Layer2::dot11_ctrl_Ack>(dst);
}

std::unique_ptr<Layer2::dot11_ctrl_BlockAck> convert(const Tins::Dot11BlockAck &src) {
  Layer2::dot11_ctrl_BlockAck *dst = new Layer2::dot11_ctrl_BlockAck;
  populateDot11Header(dst->header, src);
  return std::unique_ptr<Layer2::dot11_ctrl_BlockAck>(dst);
}

std::unique_ptr<Layer2::dot11_ctrl_BlockAckRequest> convert(const Tins::Dot11BlockAckRequest &src) {
  Layer2::dot11_ctrl_BlockAckRequest *dst = new Layer2::dot11_ctrl_BlockAckRequest;
  populateDot11Header(dst->header, src);
  return std::unique_ptr<Layer2::dot11_ctrl_BlockAckRequest>(dst);
}

std::unique_ptr<Layer2::dot11_ctrl_CfEnd> convert(const Tins::Dot11CFEnd &src) {
  Layer2::dot11_ctrl_CfEnd *dst = new Layer2::dot11_ctrl_CfEnd;
  populateDot11Header(dst->header, src);
  return std::unique_ptr<Layer2::dot11_ctrl_CfEnd>(dst);
}

std::unique_ptr<Layer2::dot11_ctrl_EndCfAck> convert(const Tins::Dot11EndCFAck &src) {
  Layer2::dot11_ctrl_EndCfAck *dst = new Layer2::dot11_ctrl_EndCfAck;
  populateDot11Header(dst->header, src);
  return std::unique_ptr<Layer2::dot11_ctrl_EndCfAck>(dst);
}

std::unique_ptr<Layer2::dot11_ctrl_PsPoll> convert(const Tins::Dot11PSPoll &src) {
  Layer2::dot11_ctrl_PsPoll *dst = new Layer2::dot11_ctrl_PsPoll;
  populateDot11Header(dst->header, src);
  return std::unique_ptr<Layer2::dot11_ctrl_PsPoll>(dst);
}

std::unique_ptr<Layer2::dot11_ctrl_Rts> convert(const Tins::Dot11RTS &src) {
  Layer2::dot11_ctrl_Rts *dst = new Layer2::dot11_ctrl_Rts;
  populateDot11Header(dst->header, src);
  return std::unique_ptr<Layer2::dot11_ctrl_Rts>(dst);
}

std::unique_ptr<Layer2::dot11_data_Data> convert(const Tins::Dot11Data &src) {
  Layer2::dot11_data_Data *dst = new Layer2::dot11_data_Data;
  populateDot11Header(dst->header, src);
  populateDot11DataHeader(dst->dataHeader, src);
  return std::unique_ptr<Layer2::dot11_data_Data>(dst);
}

std::unique_ptr<Layer2::dot11_data_QosData> convert(const Tins::Dot11QoSData &src) {
  Layer2::dot11_data_QosData *dst = new Layer2::dot11_data_QosData;
  populateDot11Header(dst->header, src);
  populateDot11DataHeader(dst->dataHeader, src);
  dst->qosControl = src.qos_control();
  return std::unique_ptr<Layer2::dot11_data_QosData>(dst);
}

std::unique_ptr<Layer2::dot11_mgmt_AssocRequest> convert(const Tins::Dot11AssocRequest &src) {
  Layer2::dot11_mgmt_AssocRequest *dst = new Layer2::dot11_mgmt_AssocRequest;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  dst->listenInterval = src.listen_interval();
  return std::unique_ptr<Layer2::dot11_mgmt_AssocRequest>(dst);
}

std::unique_ptr<Layer2::dot11_mgmt_AssocResponse> convert(const Tins::Dot11AssocResponse &src) {
  Layer2::dot11_mgmt_AssocResponse *dst = new Layer2::dot11_mgmt_AssocResponse;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  dst->statusCode = src.status_code();
  dst->aid = src.aid();
  return std::unique_ptr<Layer2::dot11_mgmt_AssocResponse>(dst);
}

std::unique_ptr<Layer2::dot11_mgmt_Authentication> convert(const Tins::Dot11Authentication &src) {
  Layer2::dot11_mgmt_Authentication *dst = new Layer2::dot11_mgmt_Authentication;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  dst->authAlgorithm = src.auth_algorithm();
  dst->authSeqNumber = src.auth_seq_number();
  dst->statusCode = src.status_code();
  return std::unique_ptr<Layer2::dot11_mgmt_Authentication>(dst);
}

std::unique_ptr<Layer2::dot11_mgmt_Beacon> convert(const Tins::Dot11Beacon &src) {
  Layer2::dot11_mgmt_Beacon *dst = new Layer2::dot11_mgmt_Beacon;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  dst->timestamp = src.timestamp();
  dst->interval = src.interval();
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  return std::unique_ptr<Layer2::dot11_mgmt_Beacon>(dst);
}

std::unique_ptr<Layer2::dot11_mgmt_Deauthentication> convert(const Tins::Dot11Deauthentication &src) {
  Layer2::dot11_mgmt_Deauthentication *dst = new Layer2::dot11_mgmt_Deauthentication;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  dst->reasonCode = src.reason_code();
  return std::unique_ptr<Layer2::dot11_mgmt_Deauthentication>(dst);
}

std::unique_ptr<Layer2::dot11_mgmt_Disassoc> convert(const Tins::Dot11Disassoc &src) {
  Layer2::dot11_mgmt_Disassoc *dst = new Layer2::dot11_mgmt_Disassoc;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  dst->reasonCode = src.reason_code();
  return std::unique_ptr<Layer2::dot11_mgmt_Disassoc>(dst);
}

std::unique_ptr<Layer2::dot11_mgmt_ProbeRequest> convert(const Tins::Dot11ProbeRequest &src) {
  Layer2::dot11_mgmt_ProbeRequest *dst = new Layer2::dot11_mgmt_ProbeRequest;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  return std::unique_ptr<Layer2::dot11_mgmt_ProbeRequest>(dst);
}

std::unique_ptr<Layer2::dot11_mgmt_ProbeResponse> convert(const Tins::Dot11ProbeResponse &src) {
  Layer2::dot11_mgmt_ProbeResponse *dst = new Layer2::dot11_mgmt_ProbeResponse;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  dst->timestamp = src.timestamp();
  dst->interval = src.interval();
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  return std::unique_ptr<Layer2::dot11_mgmt_ProbeResponse>(dst);
}

std::unique_ptr<Layer2::dot11_mgmt_ReassocRequest> convert(const Tins::Dot11ReAssocRequest &src) {
  Layer2::dot11_mgmt_ReassocRequest *dst = new Layer2::dot11_mgmt_ReassocRequest;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  dst->listenInterval = src.listen_interval();
  src.current_ap().copy(dst->currentAp.data());
  return std::unique_ptr<Layer2::dot11_mgmt_ReassocRequest>(dst);
}

std::unique_ptr<Layer2::dot11_mgmt_ReassocResponse> convert(const Tins::Dot11ReAssocResponse &src) {
  Layer2::dot11_mgmt_ReassocResponse *dst = new Layer2::dot11_mgmt_ReassocResponse;
  populateDot11Header(dst->header, src);
  populateDot11MgmtHeader(dst->mgmtHeader, src);
  populateDot11Capabilities(dst->capabilities, src.capabilities());
  dst->statusCode = src.status_code();
  dst->aid = src.aid();
  return std::unique_ptr<Layer2::dot11_mgmt_ReassocResponse>(dst);
}

std::unique_ptr<Layer2::dot11_Unsupported> convert(const Tins::Dot11 &src) {
  Layer2::dot11_Unsupported *dst = new Layer2::dot11_Unsupported;
  populateDot11Header(dst->header, src);
  return std::unique_ptr<Layer2::dot11_Unsupported>(dst);
}

// Radiotap.

std::unique_ptr<Layer2::radiotap_Radiotap> convert(const Tins::RadioTap &src) {
  Layer2::radiotap_Radiotap *dst = new Layer2::radiotap_Radiotap();

  Tins::RadioTap::PresentFlags present = src.present();
  if (present & Tins::RadioTap::PresentFlags::TSTF) {
    dst->tsft.set_long(src.tsft());
  }

  if (present & Tins::RadioTap::PresentFlags::FLAGS) {
    Tins::RadioTap::FrameFlags frameFlags = src.flags();
    std::vector<Layer2::radiotap_Flag> flags;
    if (frameFlags & Tins::RadioTap::FrameFlags::CFP) {
      flags.push_back(Layer2::radiotap_Flag::CFP);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::PREAMBLE) {
      flags.push_back(Layer2::radiotap_Flag::PREAMBLE);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::WEP) {
      flags.push_back(Layer2::radiotap_Flag::WEP);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::FRAGMENTATION) {
      flags.push_back(Layer2::radiotap_Flag::FRAGMENTATION);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::FCS) {
      flags.push_back(Layer2::radiotap_Flag::FCS);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::PADDING) {
      flags.push_back(Layer2::radiotap_Flag::PADDING);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::FAILED_FCS) {
      flags.push_back(Layer2::radiotap_Flag::FAILED_FCS);
    }
    if (frameFlags & Tins::RadioTap::FrameFlags::SHORT_GI) {
      flags.push_back(Layer2::radiotap_Flag::SHORT_GI);
    }
    dst->flags.set_array(flags);
  }

  if (present & Tins::RadioTap::PresentFlags::RATE) {
    dst->rate.set_int(src.rate());
  }

  if (present & Tins::RadioTap::PresentFlags::CHANNEL) {
    Layer2::radiotap_Channel channel;
    channel.freq = src.channel_freq();
    switch (src.channel_type()) {
    case Tins::RadioTap::TURBO:
      channel.type = Layer2::radiotap_ChannelType::TURBO;
      break;
    case Tins::RadioTap::CCK:
      channel.type = Layer2::radiotap_ChannelType::CCK;
      break;
    case Tins::RadioTap::OFDM:
      channel.type = Layer2::radiotap_ChannelType::OFDM;
      break;
    case Tins::RadioTap::TWO_GZ:
      channel.type = Layer2::radiotap_ChannelType::TWO_GZ;
      break;
    case Tins::RadioTap::FIVE_GZ:
      channel.type = Layer2::radiotap_ChannelType::FIVE_GZ;
      break;
    case Tins::RadioTap::PASSIVE:
      channel.type = Layer2::radiotap_ChannelType::PASSIVE;
      break;
    case Tins::RadioTap::DYN_CCK_OFDM:
      channel.type = Layer2::radiotap_ChannelType::DYN_CCK_OFDM;
      break;
    case Tins::RadioTap::GFSK:
      channel.type = Layer2::radiotap_ChannelType::GFSK;
      break;
    }
    dst->channel.set_radiotap_Channel(channel);
  }

  Tins::PDU *innerPdu = src.inner_pdu();
  if (innerPdu) {
    switch (innerPdu->pdu_type()) {
    case Tins::PDU::PDUType::DOT11_ACK:
      dst->pdu.set_dot11_ctrl_Ack(*Layer2::convert(static_cast<const Tins::Dot11Ack &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_BLOCK_ACK:
      dst->pdu.set_dot11_ctrl_BlockAck(*Layer2::convert(static_cast<const Tins::Dot11BlockAck &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_BLOCK_ACK_REQ:
      dst->pdu.set_dot11_ctrl_BlockAckRequest(*Layer2::convert(static_cast<const Tins::Dot11BlockAckRequest &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_CF_END:
      dst->pdu.set_dot11_ctrl_CfEnd(*Layer2::convert(static_cast<const Tins::Dot11CFEnd &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_END_CF_ACK:
      dst->pdu.set_dot11_ctrl_EndCfAck(*Layer2::convert(static_cast<const Tins::Dot11EndCFAck &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_PS_POLL:
      dst->pdu.set_dot11_ctrl_PsPoll(*Layer2::convert(static_cast<const Tins::Dot11PSPoll &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_RTS:
      dst->pdu.set_dot11_ctrl_Rts(*Layer2::convert(static_cast<const Tins::Dot11RTS &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_DATA:
      dst->pdu.set_dot11_data_Data(*Layer2::convert(static_cast<const Tins::Dot11Data &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_QOS_DATA:
      dst->pdu.set_dot11_data_QosData(*Layer2::convert(static_cast<const Tins::Dot11QoSData &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_ASSOC_REQ:
      dst->pdu.set_dot11_mgmt_AssocRequest(*Layer2::convert(static_cast<const Tins::Dot11AssocRequest &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_ASSOC_RESP:
      dst->pdu.set_dot11_mgmt_AssocResponse(*Layer2::convert(static_cast<const Tins::Dot11AssocResponse &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_AUTH:
      dst->pdu.set_dot11_mgmt_Authentication(*Layer2::convert(static_cast<const Tins::Dot11Authentication &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_BEACON:
      dst->pdu.set_dot11_mgmt_Beacon(*Layer2::convert(static_cast<const Tins::Dot11Beacon &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_DEAUTH:
      dst->pdu.set_dot11_mgmt_Deauthentication(*Layer2::convert(static_cast<const Tins::Dot11Deauthentication &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_DIASSOC:
      dst->pdu.set_dot11_mgmt_Disassoc(*Layer2::convert(static_cast<const Tins::Dot11Disassoc &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_PROBE_REQ:
      dst->pdu.set_dot11_mgmt_ProbeRequest(*Layer2::convert(static_cast<const Tins::Dot11ProbeRequest &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_PROBE_RESP:
      dst->pdu.set_dot11_mgmt_ProbeResponse(*Layer2::convert(static_cast<const Tins::Dot11ProbeResponse &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_REASSOC_REQ:
      dst->pdu.set_dot11_mgmt_ReassocRequest(*Layer2::convert(static_cast<const Tins::Dot11ReAssocRequest &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11_REASSOC_RESP:
      dst->pdu.set_dot11_mgmt_ReassocResponse(*Layer2::convert(static_cast<const Tins::Dot11ReAssocResponse &>(*innerPdu)));
      break;
    case Tins::PDU::PDUType::DOT11:
      dst->pdu.set_dot11_Unsupported(*Layer2::convert(static_cast<const Tins::Dot11 &>(*innerPdu)));
      break;
    default:
      dst->pdu.set_Unsupported(*Layer2::convert(*innerPdu));
      ; // This should never happen. Throw an exception here?
    }
  }

  return std::unique_ptr<Layer2::radiotap_Radiotap>(dst);
}

}
