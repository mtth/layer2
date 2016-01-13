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

struct Radiotap {
    typedef pdus_avsc_Union__0__ channel_t;
    int64_t tsft;
    std::vector<RadiotapFlag > flags;
    int32_t rate;
    channel_t channel;
    Radiotap() :
        tsft(int64_t()),
        flags(std::vector<RadiotapFlag >()),
        rate(int32_t()),
        channel(channel_t())
        { }
};

struct Unsupported {
    std::string name;
    std::vector<uint8_t> data;
    Unsupported() :
        name(std::string()),
        data(std::vector<uint8_t>())
        { }
};

struct pdus_avsc_Union__1__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    Unsupported get_Unsupported() const;
    void set_Unsupported(const Unsupported& v);
    Radiotap get_Radiotap() const;
    void set_Radiotap(const Radiotap& v);
    pdus_avsc_Union__1__();
};

struct Pdu {
    typedef pdus_avsc_Union__1__ pdu_t;
    pdu_t pdu;
    Pdu() :
        pdu(pdu_t())
        { }
};

struct pdus_avsc_Union__2__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    RadiotapChannelType get_RadiotapChannelType() const;
    void set_RadiotapChannelType(const RadiotapChannelType& v);
    RadiotapChannel get_RadiotapChannel() const;
    void set_RadiotapChannel(const RadiotapChannel& v);
    RadiotapFlag get_RadiotapFlag() const;
    void set_RadiotapFlag(const RadiotapFlag& v);
    Radiotap get_Radiotap() const;
    void set_Radiotap(const Radiotap& v);
    Unsupported get_Unsupported() const;
    void set_Unsupported(const Unsupported& v);
    Pdu get_Pdu() const;
    void set_Pdu(const Pdu& v);
    pdus_avsc_Union__2__();
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
    if (idx_ != 0) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Unsupported >(value_);
}

inline
void pdus_avsc_Union__1__::set_Unsupported(const Unsupported& v) {
    idx_ = 0;
    value_ = v;
}

inline
Radiotap pdus_avsc_Union__1__::get_Radiotap() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Radiotap >(value_);
}

inline
void pdus_avsc_Union__1__::set_Radiotap(const Radiotap& v) {
    idx_ = 1;
    value_ = v;
}

inline
RadiotapChannelType pdus_avsc_Union__2__::get_RadiotapChannelType() const {
    if (idx_ != 0) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<RadiotapChannelType >(value_);
}

inline
void pdus_avsc_Union__2__::set_RadiotapChannelType(const RadiotapChannelType& v) {
    idx_ = 0;
    value_ = v;
}

inline
RadiotapChannel pdus_avsc_Union__2__::get_RadiotapChannel() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<RadiotapChannel >(value_);
}

inline
void pdus_avsc_Union__2__::set_RadiotapChannel(const RadiotapChannel& v) {
    idx_ = 1;
    value_ = v;
}

inline
RadiotapFlag pdus_avsc_Union__2__::get_RadiotapFlag() const {
    if (idx_ != 2) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<RadiotapFlag >(value_);
}

inline
void pdus_avsc_Union__2__::set_RadiotapFlag(const RadiotapFlag& v) {
    idx_ = 2;
    value_ = v;
}

inline
Radiotap pdus_avsc_Union__2__::get_Radiotap() const {
    if (idx_ != 3) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Radiotap >(value_);
}

inline
void pdus_avsc_Union__2__::set_Radiotap(const Radiotap& v) {
    idx_ = 3;
    value_ = v;
}

inline
Unsupported pdus_avsc_Union__2__::get_Unsupported() const {
    if (idx_ != 4) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Unsupported >(value_);
}

inline
void pdus_avsc_Union__2__::set_Unsupported(const Unsupported& v) {
    idx_ = 4;
    value_ = v;
}

inline
Pdu pdus_avsc_Union__2__::get_Pdu() const {
    if (idx_ != 5) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Pdu >(value_);
}

inline
void pdus_avsc_Union__2__::set_Pdu(const Pdu& v) {
    idx_ = 5;
    value_ = v;
}

inline pdus_avsc_Union__0__::pdus_avsc_Union__0__() : idx_(0) { }
inline pdus_avsc_Union__1__::pdus_avsc_Union__1__() : idx_(0), value_(Unsupported()) { }
inline pdus_avsc_Union__2__::pdus_avsc_Union__2__() : idx_(0), value_(RadiotapChannelType()) { }
}
namespace avro {
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

template<> struct codec_traits<Layer2::Radiotap> {
    static void encode(Encoder& e, const Layer2::Radiotap& v) {
        avro::encode(e, v.tsft);
        avro::encode(e, v.flags);
        avro::encode(e, v.rate);
        avro::encode(e, v.channel);
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
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.tsft);
            avro::decode(d, v.flags);
            avro::decode(d, v.rate);
            avro::decode(d, v.channel);
        }
    }
};

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

template<> struct codec_traits<Layer2::pdus_avsc_Union__1__> {
    static void encode(Encoder& e, Layer2::pdus_avsc_Union__1__ v) {
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
    static void decode(Decoder& d, Layer2::pdus_avsc_Union__1__& v) {
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

template<> struct codec_traits<Layer2::pdus_avsc_Union__2__> {
    static void encode(Encoder& e, Layer2::pdus_avsc_Union__2__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            avro::encode(e, v.get_RadiotapChannelType());
            break;
        case 1:
            avro::encode(e, v.get_RadiotapChannel());
            break;
        case 2:
            avro::encode(e, v.get_RadiotapFlag());
            break;
        case 3:
            avro::encode(e, v.get_Radiotap());
            break;
        case 4:
            avro::encode(e, v.get_Unsupported());
            break;
        case 5:
            avro::encode(e, v.get_Pdu());
            break;
        }
    }
    static void decode(Decoder& d, Layer2::pdus_avsc_Union__2__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 6) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            {
                Layer2::RadiotapChannelType vv;
                avro::decode(d, vv);
                v.set_RadiotapChannelType(vv);
            }
            break;
        case 1:
            {
                Layer2::RadiotapChannel vv;
                avro::decode(d, vv);
                v.set_RadiotapChannel(vv);
            }
            break;
        case 2:
            {
                Layer2::RadiotapFlag vv;
                avro::decode(d, vv);
                v.set_RadiotapFlag(vv);
            }
            break;
        case 3:
            {
                Layer2::Radiotap vv;
                avro::decode(d, vv);
                v.set_Radiotap(vv);
            }
            break;
        case 4:
            {
                Layer2::Unsupported vv;
                avro::decode(d, vv);
                v.set_Unsupported(vv);
            }
            break;
        case 5:
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
