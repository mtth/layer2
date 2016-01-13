#include "codecs.hpp"

namespace Layer2 {

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

  return std::unique_ptr<Layer2::Radiotap>(dst);
}

std::unique_ptr<Layer2::Unsupported> convert(const Tins::PDU &src) {
  Layer2::Unsupported *dst = new Layer2::Unsupported;
  dst->name = Tins::Utils::to_string(src.pdu_type());
  dst->data = const_cast<Tins::PDU &>(src).serialize();
  return std::unique_ptr<Layer2::Unsupported>(dst);
}

}
