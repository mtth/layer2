#include "codecs.hpp"

namespace Layer2 {

std::unique_ptr<Layer2::Unsupported> convert(const Tins::PDU &src) {
  Layer2::Unsupported *dst = new Layer2::Unsupported;
  dst->name = Tins::Utils::to_string(src.pdu_type());
  dst->data = const_cast<Tins::PDU &>(src).serialize();
  return std::unique_ptr<Layer2::Unsupported>(dst);
}

std::unique_ptr<Layer2::Dot11Beacon> convert(const Tins::Dot11Beacon &src) {
  Layer2::Dot11Beacon *dst = new Layer2::Dot11Beacon;

  dst->toDs = src.to_ds();
  dst->fromDs = src.from_ds();
  dst->retry = src.retry();
  dst->powerMgmt = src.power_mgmt();
  dst->wep = src.wep();
  dst->order = src.order();
  dst->timestamp = src.timestamp();
  dst->interval = src.interval();
  dst->fragNum = src.frag_num();
  dst->seqNum = src.seq_num();
  src.addr1().copy(dst->addr1.data());
  src.addr2().copy(dst->addr2.data());
  src.addr3().copy(dst->addr3.data());
  src.addr4().copy(dst->addr4.data());

  Tins::Dot11ManagementFrame::capability_information capabilities = src.capabilities();
  if (capabilities.ess()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::ESS);
  }
  if (capabilities.ibss()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::IBSS);
  }
  if (capabilities.cf_poll()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::CF_POLL);
  }
  if (capabilities.cf_poll_req()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::CF_POLL_REQ);
  }
  if (capabilities.privacy()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::PRIVACY);
  }
  if (capabilities.short_preamble()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::SHORT_PREAMBLE);
  }
  if (capabilities.pbcc()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::PBCC);
  }
  if (capabilities.channel_agility()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::CHANNEL_AGILITY);
  }
  if (capabilities.spectrum_mgmt()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::SPECTRUM_MGMT);
  }
  if (capabilities.qos()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::QOS);
  }
  if (capabilities.sst()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::SST);
  }
  if (capabilities.apsd()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::APSD);
  }
  if (capabilities.radio_measurement()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::RADIO_MEASUREMENT);
  }
  if (capabilities.dsss_ofdm()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::DSSS_OFDM);
  }
  if (capabilities.delayed_block_ack()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::DELAYED_BLOCK_ACK);
  }
  if (capabilities.immediate_block_ack()) {
    dst->capabilities.push_back(Layer2::Dot11Capability::IMMEDIATE_BLOCK_ACK);
  }

  return std::unique_ptr<Layer2::Dot11Beacon>(dst);
}

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
    case Tins::PDU::PDUType::DOT11_BEACON:
      dst->pdu.set_Dot11Beacon(*Layer2::convert(static_cast<const Tins::Dot11Beacon &>(*innerPdu)));
      break;
    default:
      dst->pdu.set_Unsupported(*Layer2::convert(*innerPdu));
    }
  }

  return std::unique_ptr<Layer2::Radiotap>(dst);
}

}
