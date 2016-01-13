/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef SRC_PDUS_HPP_1354105471__H_
#define SRC_PDUS_HPP_1354105471__H_


#include <sstream>
#include "boost/any.hpp"
#include "../etc/deps/avro/lang/c++/api/Specific.hh"
#include "../etc/deps/avro/lang/c++/api/Encoder.hh"
#include "../etc/deps/avro/lang/c++/api/Decoder.hh"

namespace Layer2 {
struct Unsupported {
    std::string name;
    std::vector<uint8_t> data;
    Unsupported() :
        name(std::string()),
        data(std::vector<uint8_t>())
        { }
};

enum Dot11Capability {
    ESS,
    IBSS,
    CF_POLL,
    CF_POLL_REQ,
    PRIVACY,
    SHORT_PREAMBLE,
    PBCC,
    CHANNEL_AGILITY,
    SPECTRUM_MGMT,
    QOS,
    SST,
    APSD,
    RADIO_MEASUREMENT,
    DSSS_OFDM,
    DELAYED_BLOCK_ACK,
    IMMEDIATE_BLOCK_ACK,
};

struct Dot11Beacon {
    bool toDs;
    bool fromDs;
    bool moreFrag;
    bool wep;
    bool order;
    bool retry;
    bool powerMgmt;
    int32_t fragNum;
    int32_t seqNum;
    boost::array<uint8_t, 6> addr1;
    boost::array<uint8_t, 6> addr2;
    boost::array<uint8_t, 6> addr3;
    boost::array<uint8_t, 6> addr4;
    int64_t timestamp;
    int64_t interval;
    std::vector<Dot11Capability > capabilities;
    Dot11Beacon() :
        toDs(bool()),
        fromDs(bool()),
        moreFrag(bool()),
        wep(bool()),
        order(bool()),
        retry(bool()),
        powerMgmt(bool()),
        fragNum(int32_t()),
        seqNum(int32_t()),
        addr1(boost::array<uint8_t, 6>()),
        addr2(boost::array<uint8_t, 6>()),
        addr3(boost::array<uint8_t, 6>()),
        addr4(boost::array<uint8_t, 6>()),
        timestamp(int64_t()),
        interval(int64_t()),
        capabilities(std::vector<Dot11Capability >())
        { }
};

enum RadiotapChannelType {
    TURBO,
    CCK,
    OFDM,
    TWO_GZ,
    FIVE_GZ,
    PASSIVE,
    DYN_CCK_OFDM,
    GFSK,
};

struct RadiotapChannel {
    int32_t freq;
    RadiotapChannelType type;
    RadiotapChannel() :
        freq(int32_t()),
        type(RadiotapChannelType())
        { }
};

enum RadiotapFlag {
    CFP,
    PREAMBLE,
    WEP,
    FRAGMENTATION,
    FCS,
    PADDING,
    FAILED_FCS,
    SHORT_GI,
};

struct pdus_avsc_Union__0__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    bool is_null() const {
        return (idx_ == 0);
    }
    void set_null() {
        idx_ = 0;
        value_ = boost::any();
    }
    RadiotapChannel get_RadiotapChannel() const;
    void set_RadiotapChannel(const RadiotapChannel& v);
    pdus_avsc_Union__0__();
};

struct pdus_avsc_Union__1__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    bool is_null() const {
        return (idx_ == 0);
    }
    void set_null() {
        idx_ = 0;
        value_ = boost::any();
    }
    Unsupported get_Unsupported() const;
    void set_Unsupported(const Unsupported& v);
    Dot11Beacon get_Dot11Beacon() const;
    void set_Dot11Beacon(const Dot11Beacon& v);
    pdus_avsc_Union__1__();
};

struct Radiotap {
    typedef pdus_avsc_Union__0__ channel_t;
    typedef pdus_avsc_Union__1__ pdu_t;
    int64_t tsft;
    std::vector<RadiotapFlag > flags;
    int32_t rate;
    channel_t channel;
    pdu_t pdu;
    Radiotap() :
        tsft(int64_t()),
        flags(std::vector<RadiotapFlag >()),
        rate(int32_t()),
        channel(channel_t()),
        pdu(pdu_t())
        { }
};

struct pdus_avsc_Union__2__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    Unsupported get_Unsupported() const;
    void set_Unsupported(const Unsupported& v);
    Radiotap get_Radiotap() const;
    void set_Radiotap(const Radiotap& v);
    pdus_avsc_Union__2__();
};

struct Pdu {
    typedef pdus_avsc_Union__2__ pdu_t;
    pdu_t pdu;
    Pdu() :
        pdu(pdu_t())
        { }
};

struct pdus_avsc_Union__3__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    Unsupported get_Unsupported() const;
    void set_Unsupported(const Unsupported& v);
    boost::array<uint8_t, 6> get_MacAddr() const;
    void set_MacAddr(const boost::array<uint8_t, 6>& v);
    Dot11Capability get_Dot11Capability() const;
    void set_Dot11Capability(const Dot11Capability& v);
    Dot11Beacon get_Dot11Beacon() const;
    void set_Dot11Beacon(const Dot11Beacon& v);
    RadiotapChannelType get_RadiotapChannelType() const;
    void set_RadiotapChannelType(const RadiotapChannelType& v);
    RadiotapChannel get_RadiotapChannel() const;
    void set_RadiotapChannel(const RadiotapChannel& v);
    RadiotapFlag get_RadiotapFlag() const;
    void set_RadiotapFlag(const RadiotapFlag& v);
    Radiotap get_Radiotap() const;
    void set_Radiotap(const Radiotap& v);
    Pdu get_Pdu() const;
    void set_Pdu(const Pdu& v);
    pdus_avsc_Union__3__();
};

inline
RadiotapChannel pdus_avsc_Union__0__::get_RadiotapChannel() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<RadiotapChannel >(value_);
}

inline
void pdus_avsc_Union__0__::set_RadiotapChannel(const RadiotapChannel& v) {
    idx_ = 1;
    value_ = v;
}

inline
Unsupported pdus_avsc_Union__1__::get_Unsupported() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Unsupported >(value_);
}

inline
void pdus_avsc_Union__1__::set_Unsupported(const Unsupported& v) {
    idx_ = 1;
    value_ = v;
}

inline
Dot11Beacon pdus_avsc_Union__1__::get_Dot11Beacon() const {
    if (idx_ != 2) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Dot11Beacon >(value_);
}

inline
void pdus_avsc_Union__1__::set_Dot11Beacon(const Dot11Beacon& v) {
    idx_ = 2;
    value_ = v;
}

inline
Unsupported pdus_avsc_Union__2__::get_Unsupported() const {
    if (idx_ != 0) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Unsupported >(value_);
}

inline
void pdus_avsc_Union__2__::set_Unsupported(const Unsupported& v) {
    idx_ = 0;
    value_ = v;
}

inline
Radiotap pdus_avsc_Union__2__::get_Radiotap() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Radiotap >(value_);
}

inline
void pdus_avsc_Union__2__::set_Radiotap(const Radiotap& v) {
    idx_ = 1;
    value_ = v;
}

inline
Unsupported pdus_avsc_Union__3__::get_Unsupported() const {
    if (idx_ != 0) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Unsupported >(value_);
}

inline
void pdus_avsc_Union__3__::set_Unsupported(const Unsupported& v) {
    idx_ = 0;
    value_ = v;
}

inline
boost::array<uint8_t, 6> pdus_avsc_Union__3__::get_MacAddr() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<boost::array<uint8_t, 6> >(value_);
}

inline
void pdus_avsc_Union__3__::set_MacAddr(const boost::array<uint8_t, 6>& v) {
    idx_ = 1;
    value_ = v;
}

inline
Dot11Capability pdus_avsc_Union__3__::get_Dot11Capability() const {
    if (idx_ != 2) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Dot11Capability >(value_);
}

inline
void pdus_avsc_Union__3__::set_Dot11Capability(const Dot11Capability& v) {
    idx_ = 2;
    value_ = v;
}

inline
Dot11Beacon pdus_avsc_Union__3__::get_Dot11Beacon() const {
    if (idx_ != 3) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Dot11Beacon >(value_);
}

inline
void pdus_avsc_Union__3__::set_Dot11Beacon(const Dot11Beacon& v) {
    idx_ = 3;
    value_ = v;
}

inline
RadiotapChannelType pdus_avsc_Union__3__::get_RadiotapChannelType() const {
    if (idx_ != 4) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<RadiotapChannelType >(value_);
}

inline
void pdus_avsc_Union__3__::set_RadiotapChannelType(const RadiotapChannelType& v) {
    idx_ = 4;
    value_ = v;
}

inline
RadiotapChannel pdus_avsc_Union__3__::get_RadiotapChannel() const {
    if (idx_ != 5) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<RadiotapChannel >(value_);
}

inline
void pdus_avsc_Union__3__::set_RadiotapChannel(const RadiotapChannel& v) {
    idx_ = 5;
    value_ = v;
}

inline
RadiotapFlag pdus_avsc_Union__3__::get_RadiotapFlag() const {
    if (idx_ != 6) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<RadiotapFlag >(value_);
}

inline
void pdus_avsc_Union__3__::set_RadiotapFlag(const RadiotapFlag& v) {
    idx_ = 6;
    value_ = v;
}

inline
Radiotap pdus_avsc_Union__3__::get_Radiotap() const {
    if (idx_ != 7) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Radiotap >(value_);
}

inline
void pdus_avsc_Union__3__::set_Radiotap(const Radiotap& v) {
    idx_ = 7;
    value_ = v;
}

inline
Pdu pdus_avsc_Union__3__::get_Pdu() const {
    if (idx_ != 8) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Pdu >(value_);
}

inline
void pdus_avsc_Union__3__::set_Pdu(const Pdu& v) {
    idx_ = 8;
    value_ = v;
}

inline pdus_avsc_Union__0__::pdus_avsc_Union__0__() : idx_(0) { }
inline pdus_avsc_Union__1__::pdus_avsc_Union__1__() : idx_(0) { }
inline pdus_avsc_Union__2__::pdus_avsc_Union__2__() : idx_(0), value_(Unsupported()) { }
inline pdus_avsc_Union__3__::pdus_avsc_Union__3__() : idx_(0), value_(Unsupported()) { }
}
namespace avro {
template<> struct codec_traits<Layer2::Unsupported> {
    static void encode(Encoder& e, const Layer2::Unsupported& v) {
        avro::encode(e, v.name);
        avro::encode(e, v.data);
    }
    static void decode(Decoder& d, Layer2::Unsupported& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.name);
                    break;
                case 1:
                    avro::decode(d, v.data);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.name);
            avro::decode(d, v.data);
        }
    }
};

template<> struct codec_traits<Layer2::Dot11Capability> {
    static void encode(Encoder& e, Layer2::Dot11Capability v) {
		if (v < Layer2::ESS || v > Layer2::IMMEDIATE_BLOCK_ACK)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for Layer2::Dot11Capability and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, Layer2::Dot11Capability& v) {
		size_t index = d.decodeEnum();
		if (index < Layer2::ESS || index > Layer2::IMMEDIATE_BLOCK_ACK)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for Layer2::Dot11Capability and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<Layer2::Dot11Capability>(index);
    }
};

template<> struct codec_traits<Layer2::Dot11Beacon> {
    static void encode(Encoder& e, const Layer2::Dot11Beacon& v) {
        avro::encode(e, v.toDs);
        avro::encode(e, v.fromDs);
        avro::encode(e, v.moreFrag);
        avro::encode(e, v.wep);
        avro::encode(e, v.order);
        avro::encode(e, v.retry);
        avro::encode(e, v.powerMgmt);
        avro::encode(e, v.fragNum);
        avro::encode(e, v.seqNum);
        avro::encode(e, v.addr1);
        avro::encode(e, v.addr2);
        avro::encode(e, v.addr3);
        avro::encode(e, v.addr4);
        avro::encode(e, v.timestamp);
        avro::encode(e, v.interval);
        avro::encode(e, v.capabilities);
    }
    static void decode(Decoder& d, Layer2::Dot11Beacon& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.toDs);
                    break;
                case 1:
                    avro::decode(d, v.fromDs);
                    break;
                case 2:
                    avro::decode(d, v.moreFrag);
                    break;
                case 3:
                    avro::decode(d, v.wep);
                    break;
                case 4:
                    avro::decode(d, v.order);
                    break;
                case 5:
                    avro::decode(d, v.retry);
                    break;
                case 6:
                    avro::decode(d, v.powerMgmt);
                    break;
                case 7:
                    avro::decode(d, v.fragNum);
                    break;
                case 8:
                    avro::decode(d, v.seqNum);
                    break;
                case 9:
                    avro::decode(d, v.addr1);
                    break;
                case 10:
                    avro::decode(d, v.addr2);
                    break;
                case 11:
                    avro::decode(d, v.addr3);
                    break;
                case 12:
                    avro::decode(d, v.addr4);
                    break;
                case 13:
                    avro::decode(d, v.timestamp);
                    break;
                case 14:
                    avro::decode(d, v.interval);
                    break;
                case 15:
                    avro::decode(d, v.capabilities);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.toDs);
            avro::decode(d, v.fromDs);
            avro::decode(d, v.moreFrag);
            avro::decode(d, v.wep);
            avro::decode(d, v.order);
            avro::decode(d, v.retry);
            avro::decode(d, v.powerMgmt);
            avro::decode(d, v.fragNum);
            avro::decode(d, v.seqNum);
            avro::decode(d, v.addr1);
            avro::decode(d, v.addr2);
            avro::decode(d, v.addr3);
            avro::decode(d, v.addr4);
            avro::decode(d, v.timestamp);
            avro::decode(d, v.interval);
            avro::decode(d, v.capabilities);
        }
    }
};

template<> struct codec_traits<Layer2::RadiotapChannelType> {
    static void encode(Encoder& e, Layer2::RadiotapChannelType v) {
		if (v < Layer2::TURBO || v > Layer2::GFSK)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for Layer2::RadiotapChannelType and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, Layer2::RadiotapChannelType& v) {
		size_t index = d.decodeEnum();
		if (index < Layer2::TURBO || index > Layer2::GFSK)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for Layer2::RadiotapChannelType and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<Layer2::RadiotapChannelType>(index);
    }
};

template<> struct codec_traits<Layer2::RadiotapChannel> {
    static void encode(Encoder& e, const Layer2::RadiotapChannel& v) {
        avro::encode(e, v.freq);
        avro::encode(e, v.type);
    }
    static void decode(Decoder& d, Layer2::RadiotapChannel& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.freq);
                    break;
                case 1:
                    avro::decode(d, v.type);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.freq);
            avro::decode(d, v.type);
        }
    }
};

template<> struct codec_traits<Layer2::RadiotapFlag> {
    static void encode(Encoder& e, Layer2::RadiotapFlag v) {
		if (v < Layer2::CFP || v > Layer2::SHORT_GI)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for Layer2::RadiotapFlag and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, Layer2::RadiotapFlag& v) {
		size_t index = d.decodeEnum();
		if (index < Layer2::CFP || index > Layer2::SHORT_GI)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for Layer2::RadiotapFlag and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<Layer2::RadiotapFlag>(index);
    }
};

template<> struct codec_traits<Layer2::pdus_avsc_Union__0__> {
    static void encode(Encoder& e, Layer2::pdus_avsc_Union__0__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            e.encodeNull();
            break;
        case 1:
            avro::encode(e, v.get_RadiotapChannel());
            break;
        }
    }
    static void decode(Decoder& d, Layer2::pdus_avsc_Union__0__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 2) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            d.decodeNull();
            v.set_null();
            break;
        case 1:
            {
                Layer2::RadiotapChannel vv;
                avro::decode(d, vv);
                v.set_RadiotapChannel(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<Layer2::pdus_avsc_Union__1__> {
    static void encode(Encoder& e, Layer2::pdus_avsc_Union__1__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            e.encodeNull();
            break;
        case 1:
            avro::encode(e, v.get_Unsupported());
            break;
        case 2:
            avro::encode(e, v.get_Dot11Beacon());
            break;
        }
    }
    static void decode(Decoder& d, Layer2::pdus_avsc_Union__1__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 3) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            d.decodeNull();
            v.set_null();
            break;
        case 1:
            {
                Layer2::Unsupported vv;
                avro::decode(d, vv);
                v.set_Unsupported(vv);
            }
            break;
        case 2:
            {
                Layer2::Dot11Beacon vv;
                avro::decode(d, vv);
                v.set_Dot11Beacon(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<Layer2::Radiotap> {
    static void encode(Encoder& e, const Layer2::Radiotap& v) {
        avro::encode(e, v.tsft);
        avro::encode(e, v.flags);
        avro::encode(e, v.rate);
        avro::encode(e, v.channel);
        avro::encode(e, v.pdu);
    }
    static void decode(Decoder& d, Layer2::Radiotap& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.tsft);
                    break;
                case 1:
                    avro::decode(d, v.flags);
                    break;
                case 2:
                    avro::decode(d, v.rate);
                    break;
                case 3:
                    avro::decode(d, v.channel);
                    break;
                case 4:
                    avro::decode(d, v.pdu);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.tsft);
            avro::decode(d, v.flags);
            avro::decode(d, v.rate);
            avro::decode(d, v.channel);
            avro::decode(d, v.pdu);
        }
    }
};

template<> struct codec_traits<Layer2::pdus_avsc_Union__2__> {
    static void encode(Encoder& e, Layer2::pdus_avsc_Union__2__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            avro::encode(e, v.get_Unsupported());
            break;
        case 1:
            avro::encode(e, v.get_Radiotap());
            break;
        }
    }
    static void decode(Decoder& d, Layer2::pdus_avsc_Union__2__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 2) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            {
                Layer2::Unsupported vv;
                avro::decode(d, vv);
                v.set_Unsupported(vv);
            }
            break;
        case 1:
            {
                Layer2::Radiotap vv;
                avro::decode(d, vv);
                v.set_Radiotap(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<Layer2::Pdu> {
    static void encode(Encoder& e, const Layer2::Pdu& v) {
        avro::encode(e, v.pdu);
    }
    static void decode(Decoder& d, Layer2::Pdu& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.pdu);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.pdu);
        }
    }
};

template<> struct codec_traits<Layer2::pdus_avsc_Union__3__> {
    static void encode(Encoder& e, Layer2::pdus_avsc_Union__3__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            avro::encode(e, v.get_Unsupported());
            break;
        case 1:
            avro::encode(e, v.get_MacAddr());
            break;
        case 2:
            avro::encode(e, v.get_Dot11Capability());
            break;
        case 3:
            avro::encode(e, v.get_Dot11Beacon());
            break;
        case 4:
            avro::encode(e, v.get_RadiotapChannelType());
            break;
        case 5:
            avro::encode(e, v.get_RadiotapChannel());
            break;
        case 6:
            avro::encode(e, v.get_RadiotapFlag());
            break;
        case 7:
            avro::encode(e, v.get_Radiotap());
            break;
        case 8:
            avro::encode(e, v.get_Pdu());
            break;
        }
    }
    static void decode(Decoder& d, Layer2::pdus_avsc_Union__3__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 9) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            {
                Layer2::Unsupported vv;
                avro::decode(d, vv);
                v.set_Unsupported(vv);
            }
            break;
        case 1:
            {
                boost::array<uint8_t, 6> vv;
                avro::decode(d, vv);
                v.set_MacAddr(vv);
            }
            break;
        case 2:
            {
                Layer2::Dot11Capability vv;
                avro::decode(d, vv);
                v.set_Dot11Capability(vv);
            }
            break;
        case 3:
            {
                Layer2::Dot11Beacon vv;
                avro::decode(d, vv);
                v.set_Dot11Beacon(vv);
            }
            break;
        case 4:
            {
                Layer2::RadiotapChannelType vv;
                avro::decode(d, vv);
                v.set_RadiotapChannelType(vv);
            }
            break;
        case 5:
            {
                Layer2::RadiotapChannel vv;
                avro::decode(d, vv);
                v.set_RadiotapChannel(vv);
            }
            break;
        case 6:
            {
                Layer2::RadiotapFlag vv;
                avro::decode(d, vv);
                v.set_RadiotapFlag(vv);
            }
            break;
        case 7:
            {
                Layer2::Radiotap vv;
                avro::decode(d, vv);
                v.set_Radiotap(vv);
            }
            break;
        case 8:
            {
                Layer2::Pdu vv;
                avro::decode(d, vv);
                v.set_Pdu(vv);
            }
            break;
        }
    }
};

}
#endif
