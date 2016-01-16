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


#ifndef SRC_PDUS_HPP_4119412750__H_
#define SRC_PDUS_HPP_4119412750__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace Layer2 {
struct Unsupported {
    std::string name;
    std::vector<uint8_t> data;
    Unsupported() :
        name(std::string()),
        data(std::vector<uint8_t>())
        { }
};

struct ethernet2_Ethernet2 {
    boost::array<uint8_t, 6> srcAddr;
    boost::array<uint8_t, 6> dstAddr;
    int32_t payloadType;
    std::vector<uint8_t> data;
    ethernet2_Ethernet2() :
        srcAddr(boost::array<uint8_t, 6>()),
        dstAddr(boost::array<uint8_t, 6>()),
        payloadType(int32_t()),
        data(std::vector<uint8_t>())
        { }
};

struct dot11_Header {
    bool toDs;
    bool fromDs;
    bool moreFrag;
    bool retry;
    bool powerMgmt;
    bool wep;
    bool order;
    int32_t durationId;
    boost::array<uint8_t, 6> addr1;
    dot11_Header() :
        toDs(bool()),
        fromDs(bool()),
        moreFrag(bool()),
        retry(bool()),
        powerMgmt(bool()),
        wep(bool()),
        order(bool()),
        durationId(int32_t()),
        addr1(boost::array<uint8_t, 6>())
        { }
};

struct dot11_Unsupported {
    int32_t type;
    int32_t subtype;
    dot11_Unsupported() :
        type(int32_t()),
        subtype(int32_t())
        { }
};

struct dot11_ctrl_Ack {
    dot11_Header header;
    dot11_ctrl_Ack() :
        header(dot11_Header())
        { }
};

struct dot11_ctrl_BlockAck {
    dot11_Header header;
    int32_t barControl;
    int32_t startSeq;
    int32_t fragNum;
    dot11_ctrl_BlockAck() :
        header(dot11_Header()),
        barControl(int32_t()),
        startSeq(int32_t()),
        fragNum(int32_t())
        { }
};

struct dot11_ctrl_BlockAckRequest {
    dot11_Header header;
    int32_t barControl;
    int32_t startSeq;
    int32_t fragNum;
    dot11_ctrl_BlockAckRequest() :
        header(dot11_Header()),
        barControl(int32_t()),
        startSeq(int32_t()),
        fragNum(int32_t())
        { }
};

struct dot11_ctrl_CfEnd {
    dot11_Header header;
    dot11_ctrl_CfEnd() :
        header(dot11_Header())
        { }
};

struct dot11_ctrl_EndCfAck {
    dot11_Header header;
    dot11_ctrl_EndCfAck() :
        header(dot11_Header())
        { }
};

struct dot11_ctrl_PsPoll {
    dot11_Header header;
    dot11_ctrl_PsPoll() :
        header(dot11_Header())
        { }
};

struct dot11_ctrl_Rts {
    dot11_Header header;
    dot11_ctrl_Rts() :
        header(dot11_Header())
        { }
};

struct dot11_data_Header {
    boost::array<uint8_t, 6> addr2;
    boost::array<uint8_t, 6> addr3;
    boost::array<uint8_t, 6> addr4;
    int32_t fragNum;
    int32_t seqNum;
    dot11_data_Header() :
        addr2(boost::array<uint8_t, 6>()),
        addr3(boost::array<uint8_t, 6>()),
        addr4(boost::array<uint8_t, 6>()),
        fragNum(int32_t()),
        seqNum(int32_t())
        { }
};

struct dot11_data_Data {
    dot11_Header header;
    dot11_data_Header dataHeader;
    dot11_data_Data() :
        header(dot11_Header()),
        dataHeader(dot11_data_Header())
        { }
};

struct dot11_data_QosData {
    dot11_Header header;
    dot11_data_Header dataHeader;
    int32_t qosControl;
    dot11_data_QosData() :
        header(dot11_Header()),
        dataHeader(dot11_data_Header()),
        qosControl(int32_t())
        { }
};

enum dot11_mgmt_Capability {
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

struct dot11_mgmt_Header {
    boost::array<uint8_t, 6> addr2;
    boost::array<uint8_t, 6> addr3;
    boost::array<uint8_t, 6> addr4;
    int32_t fragNum;
    int32_t seqNum;
    dot11_mgmt_Header() :
        addr2(boost::array<uint8_t, 6>()),
        addr3(boost::array<uint8_t, 6>()),
        addr4(boost::array<uint8_t, 6>()),
        fragNum(int32_t()),
        seqNum(int32_t())
        { }
};

struct dot11_mgmt_AssocRequest {
    dot11_Header header;
    dot11_mgmt_Header mgmtHeader;
    std::vector<dot11_mgmt_Capability > capabilities;
    int32_t listenInterval;
    dot11_mgmt_AssocRequest() :
        header(dot11_Header()),
        mgmtHeader(dot11_mgmt_Header()),
        capabilities(std::vector<dot11_mgmt_Capability >()),
        listenInterval(int32_t())
        { }
};

struct dot11_mgmt_AssocResponse {
    dot11_Header header;
    dot11_mgmt_Header mgmtHeader;
    std::vector<dot11_mgmt_Capability > capabilities;
    int32_t statusCode;
    int32_t aid;
    dot11_mgmt_AssocResponse() :
        header(dot11_Header()),
        mgmtHeader(dot11_mgmt_Header()),
        capabilities(std::vector<dot11_mgmt_Capability >()),
        statusCode(int32_t()),
        aid(int32_t())
        { }
};

struct dot11_mgmt_Authentication {
    dot11_Header header;
    dot11_mgmt_Header mgmtHeader;
    int32_t authAlgorithm;
    int32_t authSeqNumber;
    int32_t statusCode;
    dot11_mgmt_Authentication() :
        header(dot11_Header()),
        mgmtHeader(dot11_mgmt_Header()),
        authAlgorithm(int32_t()),
        authSeqNumber(int32_t()),
        statusCode(int32_t())
        { }
};

struct dot11_mgmt_Beacon {
    dot11_Header header;
    dot11_mgmt_Header mgmtHeader;
    int64_t timestamp;
    int32_t interval;
    std::vector<dot11_mgmt_Capability > capabilities;
    dot11_mgmt_Beacon() :
        header(dot11_Header()),
        mgmtHeader(dot11_mgmt_Header()),
        timestamp(int64_t()),
        interval(int32_t()),
        capabilities(std::vector<dot11_mgmt_Capability >())
        { }
};

struct dot11_mgmt_Deauthentication {
    dot11_Header header;
    dot11_mgmt_Header mgmtHeader;
    int32_t reasonCode;
    dot11_mgmt_Deauthentication() :
        header(dot11_Header()),
        mgmtHeader(dot11_mgmt_Header()),
        reasonCode(int32_t())
        { }
};

struct dot11_mgmt_Disassoc {
    dot11_Header header;
    dot11_mgmt_Header mgmtHeader;
    int32_t reasonCode;
    dot11_mgmt_Disassoc() :
        header(dot11_Header()),
        mgmtHeader(dot11_mgmt_Header()),
        reasonCode(int32_t())
        { }
};

struct dot11_mgmt_ProbeRequest {
    dot11_Header header;
    dot11_mgmt_Header mgmtHeader;
    dot11_mgmt_ProbeRequest() :
        header(dot11_Header()),
        mgmtHeader(dot11_mgmt_Header())
        { }
};

struct dot11_mgmt_ProbeResponse {
    dot11_Header header;
    dot11_mgmt_Header mgmtHeader;
    int64_t timestamp;
    int32_t interval;
    std::vector<dot11_mgmt_Capability > capabilities;
    dot11_mgmt_ProbeResponse() :
        header(dot11_Header()),
        mgmtHeader(dot11_mgmt_Header()),
        timestamp(int64_t()),
        interval(int32_t()),
        capabilities(std::vector<dot11_mgmt_Capability >())
        { }
};

struct dot11_mgmt_ReassocRequest {
    dot11_Header header;
    dot11_mgmt_Header mgmtHeader;
    std::vector<dot11_mgmt_Capability > capabilities;
    int32_t listenInterval;
    boost::array<uint8_t, 6> currentAp;
    dot11_mgmt_ReassocRequest() :
        header(dot11_Header()),
        mgmtHeader(dot11_mgmt_Header()),
        capabilities(std::vector<dot11_mgmt_Capability >()),
        listenInterval(int32_t()),
        currentAp(boost::array<uint8_t, 6>())
        { }
};

struct dot11_mgmt_ReassocResponse {
    dot11_Header header;
    dot11_mgmt_Header mgmtHeader;
    std::vector<dot11_mgmt_Capability > capabilities;
    int32_t statusCode;
    int32_t aid;
    dot11_mgmt_ReassocResponse() :
        header(dot11_Header()),
        mgmtHeader(dot11_mgmt_Header()),
        capabilities(std::vector<dot11_mgmt_Capability >()),
        statusCode(int32_t()),
        aid(int32_t())
        { }
};

enum radiotap_Flag {
    CFP,
    PREAMBLE,
    WEP,
    FRAGMENTATION,
    FCS,
    PADDING,
    FAILED_FCS,
    SHORT_GI,
};

enum radiotap_ChannelType {
    TURBO,
    CCK,
    OFDM,
    TWO_GZ,
    FIVE_GZ,
    PASSIVE,
    DYN_CCK_OFDM,
    GFSK,
};

struct radiotap_Channel {
    int32_t freq;
    radiotap_ChannelType type;
    radiotap_Channel() :
        freq(int32_t()),
        type(radiotap_ChannelType())
        { }
};

struct _63_Union__0__ {
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
    radiotap_Channel get_radiotap_Channel() const;
    void set_radiotap_Channel(const radiotap_Channel& v);
    _63_Union__0__();
};

struct _63_Union__1__ {
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
    dot11_Unsupported get_dot11_Unsupported() const;
    void set_dot11_Unsupported(const dot11_Unsupported& v);
    dot11_ctrl_Ack get_dot11_ctrl_Ack() const;
    void set_dot11_ctrl_Ack(const dot11_ctrl_Ack& v);
    dot11_ctrl_BlockAck get_dot11_ctrl_BlockAck() const;
    void set_dot11_ctrl_BlockAck(const dot11_ctrl_BlockAck& v);
    dot11_ctrl_BlockAckRequest get_dot11_ctrl_BlockAckRequest() const;
    void set_dot11_ctrl_BlockAckRequest(const dot11_ctrl_BlockAckRequest& v);
    dot11_ctrl_CfEnd get_dot11_ctrl_CfEnd() const;
    void set_dot11_ctrl_CfEnd(const dot11_ctrl_CfEnd& v);
    dot11_ctrl_EndCfAck get_dot11_ctrl_EndCfAck() const;
    void set_dot11_ctrl_EndCfAck(const dot11_ctrl_EndCfAck& v);
    dot11_ctrl_PsPoll get_dot11_ctrl_PsPoll() const;
    void set_dot11_ctrl_PsPoll(const dot11_ctrl_PsPoll& v);
    dot11_ctrl_Rts get_dot11_ctrl_Rts() const;
    void set_dot11_ctrl_Rts(const dot11_ctrl_Rts& v);
    dot11_data_Data get_dot11_data_Data() const;
    void set_dot11_data_Data(const dot11_data_Data& v);
    dot11_data_QosData get_dot11_data_QosData() const;
    void set_dot11_data_QosData(const dot11_data_QosData& v);
    dot11_mgmt_AssocRequest get_dot11_mgmt_AssocRequest() const;
    void set_dot11_mgmt_AssocRequest(const dot11_mgmt_AssocRequest& v);
    dot11_mgmt_AssocResponse get_dot11_mgmt_AssocResponse() const;
    void set_dot11_mgmt_AssocResponse(const dot11_mgmt_AssocResponse& v);
    dot11_mgmt_Authentication get_dot11_mgmt_Authentication() const;
    void set_dot11_mgmt_Authentication(const dot11_mgmt_Authentication& v);
    dot11_mgmt_Beacon get_dot11_mgmt_Beacon() const;
    void set_dot11_mgmt_Beacon(const dot11_mgmt_Beacon& v);
    dot11_mgmt_Deauthentication get_dot11_mgmt_Deauthentication() const;
    void set_dot11_mgmt_Deauthentication(const dot11_mgmt_Deauthentication& v);
    dot11_mgmt_Disassoc get_dot11_mgmt_Disassoc() const;
    void set_dot11_mgmt_Disassoc(const dot11_mgmt_Disassoc& v);
    dot11_mgmt_ProbeRequest get_dot11_mgmt_ProbeRequest() const;
    void set_dot11_mgmt_ProbeRequest(const dot11_mgmt_ProbeRequest& v);
    dot11_mgmt_ProbeResponse get_dot11_mgmt_ProbeResponse() const;
    void set_dot11_mgmt_ProbeResponse(const dot11_mgmt_ProbeResponse& v);
    dot11_mgmt_ReassocRequest get_dot11_mgmt_ReassocRequest() const;
    void set_dot11_mgmt_ReassocRequest(const dot11_mgmt_ReassocRequest& v);
    dot11_mgmt_ReassocResponse get_dot11_mgmt_ReassocResponse() const;
    void set_dot11_mgmt_ReassocResponse(const dot11_mgmt_ReassocResponse& v);
    _63_Union__1__();
};

struct radiotap_Radiotap {
    typedef _63_Union__0__ channel_t;
    typedef _63_Union__1__ pdu_t;
    int64_t tsft;
    std::vector<radiotap_Flag > flags;
    int32_t rate;
    channel_t channel;
    pdu_t pdu;
    radiotap_Radiotap() :
        tsft(int64_t()),
        flags(std::vector<radiotap_Flag >()),
        rate(int32_t()),
        channel(channel_t()),
        pdu(pdu_t())
        { }
};

struct _63_Union__2__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    Unsupported get_Unsupported() const;
    void set_Unsupported(const Unsupported& v);
    ethernet2_Ethernet2 get_ethernet2_Ethernet2() const;
    void set_ethernet2_Ethernet2(const ethernet2_Ethernet2& v);
    radiotap_Radiotap get_radiotap_Radiotap() const;
    void set_radiotap_Radiotap(const radiotap_Radiotap& v);
    _63_Union__2__();
};

struct Pdu {
    typedef _63_Union__2__ pdu_t;
    pdu_t pdu;
    Pdu() :
        pdu(pdu_t())
        { }
};

struct _63_Union__3__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    Unsupported get_Unsupported() const;
    void set_Unsupported(const Unsupported& v);
    boost::array<uint8_t, 6> get_MacAddr() const;
    void set_MacAddr(const boost::array<uint8_t, 6>& v);
    ethernet2_Ethernet2 get_ethernet2_Ethernet2() const;
    void set_ethernet2_Ethernet2(const ethernet2_Ethernet2& v);
    dot11_Header get_dot11_Header() const;
    void set_dot11_Header(const dot11_Header& v);
    dot11_Unsupported get_dot11_Unsupported() const;
    void set_dot11_Unsupported(const dot11_Unsupported& v);
    dot11_ctrl_Ack get_dot11_ctrl_Ack() const;
    void set_dot11_ctrl_Ack(const dot11_ctrl_Ack& v);
    dot11_ctrl_BlockAck get_dot11_ctrl_BlockAck() const;
    void set_dot11_ctrl_BlockAck(const dot11_ctrl_BlockAck& v);
    dot11_ctrl_BlockAckRequest get_dot11_ctrl_BlockAckRequest() const;
    void set_dot11_ctrl_BlockAckRequest(const dot11_ctrl_BlockAckRequest& v);
    dot11_ctrl_CfEnd get_dot11_ctrl_CfEnd() const;
    void set_dot11_ctrl_CfEnd(const dot11_ctrl_CfEnd& v);
    dot11_ctrl_EndCfAck get_dot11_ctrl_EndCfAck() const;
    void set_dot11_ctrl_EndCfAck(const dot11_ctrl_EndCfAck& v);
    dot11_ctrl_PsPoll get_dot11_ctrl_PsPoll() const;
    void set_dot11_ctrl_PsPoll(const dot11_ctrl_PsPoll& v);
    dot11_ctrl_Rts get_dot11_ctrl_Rts() const;
    void set_dot11_ctrl_Rts(const dot11_ctrl_Rts& v);
    dot11_data_Header get_dot11_data_Header() const;
    void set_dot11_data_Header(const dot11_data_Header& v);
    dot11_data_Data get_dot11_data_Data() const;
    void set_dot11_data_Data(const dot11_data_Data& v);
    dot11_data_QosData get_dot11_data_QosData() const;
    void set_dot11_data_QosData(const dot11_data_QosData& v);
    dot11_mgmt_Capability get_dot11_mgmt_Capability() const;
    void set_dot11_mgmt_Capability(const dot11_mgmt_Capability& v);
    dot11_mgmt_Header get_dot11_mgmt_Header() const;
    void set_dot11_mgmt_Header(const dot11_mgmt_Header& v);
    dot11_mgmt_AssocRequest get_dot11_mgmt_AssocRequest() const;
    void set_dot11_mgmt_AssocRequest(const dot11_mgmt_AssocRequest& v);
    dot11_mgmt_AssocResponse get_dot11_mgmt_AssocResponse() const;
    void set_dot11_mgmt_AssocResponse(const dot11_mgmt_AssocResponse& v);
    dot11_mgmt_Authentication get_dot11_mgmt_Authentication() const;
    void set_dot11_mgmt_Authentication(const dot11_mgmt_Authentication& v);
    dot11_mgmt_Beacon get_dot11_mgmt_Beacon() const;
    void set_dot11_mgmt_Beacon(const dot11_mgmt_Beacon& v);
    dot11_mgmt_Deauthentication get_dot11_mgmt_Deauthentication() const;
    void set_dot11_mgmt_Deauthentication(const dot11_mgmt_Deauthentication& v);
    dot11_mgmt_Disassoc get_dot11_mgmt_Disassoc() const;
    void set_dot11_mgmt_Disassoc(const dot11_mgmt_Disassoc& v);
    dot11_mgmt_ProbeRequest get_dot11_mgmt_ProbeRequest() const;
    void set_dot11_mgmt_ProbeRequest(const dot11_mgmt_ProbeRequest& v);
    dot11_mgmt_ProbeResponse get_dot11_mgmt_ProbeResponse() const;
    void set_dot11_mgmt_ProbeResponse(const dot11_mgmt_ProbeResponse& v);
    dot11_mgmt_ReassocRequest get_dot11_mgmt_ReassocRequest() const;
    void set_dot11_mgmt_ReassocRequest(const dot11_mgmt_ReassocRequest& v);
    dot11_mgmt_ReassocResponse get_dot11_mgmt_ReassocResponse() const;
    void set_dot11_mgmt_ReassocResponse(const dot11_mgmt_ReassocResponse& v);
    radiotap_Flag get_radiotap_Flag() const;
    void set_radiotap_Flag(const radiotap_Flag& v);
    radiotap_ChannelType get_radiotap_ChannelType() const;
    void set_radiotap_ChannelType(const radiotap_ChannelType& v);
    radiotap_Channel get_radiotap_Channel() const;
    void set_radiotap_Channel(const radiotap_Channel& v);
    radiotap_Radiotap get_radiotap_Radiotap() const;
    void set_radiotap_Radiotap(const radiotap_Radiotap& v);
    Pdu get_Pdu() const;
    void set_Pdu(const Pdu& v);
    _63_Union__3__();
};

inline
radiotap_Channel _63_Union__0__::get_radiotap_Channel() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<radiotap_Channel >(value_);
}

inline
void _63_Union__0__::set_radiotap_Channel(const radiotap_Channel& v) {
    idx_ = 1;
    value_ = v;
}

inline
Unsupported _63_Union__1__::get_Unsupported() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Unsupported >(value_);
}

inline
void _63_Union__1__::set_Unsupported(const Unsupported& v) {
    idx_ = 1;
    value_ = v;
}

inline
dot11_Unsupported _63_Union__1__::get_dot11_Unsupported() const {
    if (idx_ != 2) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_Unsupported >(value_);
}

inline
void _63_Union__1__::set_dot11_Unsupported(const dot11_Unsupported& v) {
    idx_ = 2;
    value_ = v;
}

inline
dot11_ctrl_Ack _63_Union__1__::get_dot11_ctrl_Ack() const {
    if (idx_ != 3) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_Ack >(value_);
}

inline
void _63_Union__1__::set_dot11_ctrl_Ack(const dot11_ctrl_Ack& v) {
    idx_ = 3;
    value_ = v;
}

inline
dot11_ctrl_BlockAck _63_Union__1__::get_dot11_ctrl_BlockAck() const {
    if (idx_ != 4) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_BlockAck >(value_);
}

inline
void _63_Union__1__::set_dot11_ctrl_BlockAck(const dot11_ctrl_BlockAck& v) {
    idx_ = 4;
    value_ = v;
}

inline
dot11_ctrl_BlockAckRequest _63_Union__1__::get_dot11_ctrl_BlockAckRequest() const {
    if (idx_ != 5) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_BlockAckRequest >(value_);
}

inline
void _63_Union__1__::set_dot11_ctrl_BlockAckRequest(const dot11_ctrl_BlockAckRequest& v) {
    idx_ = 5;
    value_ = v;
}

inline
dot11_ctrl_CfEnd _63_Union__1__::get_dot11_ctrl_CfEnd() const {
    if (idx_ != 6) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_CfEnd >(value_);
}

inline
void _63_Union__1__::set_dot11_ctrl_CfEnd(const dot11_ctrl_CfEnd& v) {
    idx_ = 6;
    value_ = v;
}

inline
dot11_ctrl_EndCfAck _63_Union__1__::get_dot11_ctrl_EndCfAck() const {
    if (idx_ != 7) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_EndCfAck >(value_);
}

inline
void _63_Union__1__::set_dot11_ctrl_EndCfAck(const dot11_ctrl_EndCfAck& v) {
    idx_ = 7;
    value_ = v;
}

inline
dot11_ctrl_PsPoll _63_Union__1__::get_dot11_ctrl_PsPoll() const {
    if (idx_ != 8) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_PsPoll >(value_);
}

inline
void _63_Union__1__::set_dot11_ctrl_PsPoll(const dot11_ctrl_PsPoll& v) {
    idx_ = 8;
    value_ = v;
}

inline
dot11_ctrl_Rts _63_Union__1__::get_dot11_ctrl_Rts() const {
    if (idx_ != 9) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_Rts >(value_);
}

inline
void _63_Union__1__::set_dot11_ctrl_Rts(const dot11_ctrl_Rts& v) {
    idx_ = 9;
    value_ = v;
}

inline
dot11_data_Data _63_Union__1__::get_dot11_data_Data() const {
    if (idx_ != 10) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_data_Data >(value_);
}

inline
void _63_Union__1__::set_dot11_data_Data(const dot11_data_Data& v) {
    idx_ = 10;
    value_ = v;
}

inline
dot11_data_QosData _63_Union__1__::get_dot11_data_QosData() const {
    if (idx_ != 11) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_data_QosData >(value_);
}

inline
void _63_Union__1__::set_dot11_data_QosData(const dot11_data_QosData& v) {
    idx_ = 11;
    value_ = v;
}

inline
dot11_mgmt_AssocRequest _63_Union__1__::get_dot11_mgmt_AssocRequest() const {
    if (idx_ != 12) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_AssocRequest >(value_);
}

inline
void _63_Union__1__::set_dot11_mgmt_AssocRequest(const dot11_mgmt_AssocRequest& v) {
    idx_ = 12;
    value_ = v;
}

inline
dot11_mgmt_AssocResponse _63_Union__1__::get_dot11_mgmt_AssocResponse() const {
    if (idx_ != 13) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_AssocResponse >(value_);
}

inline
void _63_Union__1__::set_dot11_mgmt_AssocResponse(const dot11_mgmt_AssocResponse& v) {
    idx_ = 13;
    value_ = v;
}

inline
dot11_mgmt_Authentication _63_Union__1__::get_dot11_mgmt_Authentication() const {
    if (idx_ != 14) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_Authentication >(value_);
}

inline
void _63_Union__1__::set_dot11_mgmt_Authentication(const dot11_mgmt_Authentication& v) {
    idx_ = 14;
    value_ = v;
}

inline
dot11_mgmt_Beacon _63_Union__1__::get_dot11_mgmt_Beacon() const {
    if (idx_ != 15) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_Beacon >(value_);
}

inline
void _63_Union__1__::set_dot11_mgmt_Beacon(const dot11_mgmt_Beacon& v) {
    idx_ = 15;
    value_ = v;
}

inline
dot11_mgmt_Deauthentication _63_Union__1__::get_dot11_mgmt_Deauthentication() const {
    if (idx_ != 16) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_Deauthentication >(value_);
}

inline
void _63_Union__1__::set_dot11_mgmt_Deauthentication(const dot11_mgmt_Deauthentication& v) {
    idx_ = 16;
    value_ = v;
}

inline
dot11_mgmt_Disassoc _63_Union__1__::get_dot11_mgmt_Disassoc() const {
    if (idx_ != 17) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_Disassoc >(value_);
}

inline
void _63_Union__1__::set_dot11_mgmt_Disassoc(const dot11_mgmt_Disassoc& v) {
    idx_ = 17;
    value_ = v;
}

inline
dot11_mgmt_ProbeRequest _63_Union__1__::get_dot11_mgmt_ProbeRequest() const {
    if (idx_ != 18) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_ProbeRequest >(value_);
}

inline
void _63_Union__1__::set_dot11_mgmt_ProbeRequest(const dot11_mgmt_ProbeRequest& v) {
    idx_ = 18;
    value_ = v;
}

inline
dot11_mgmt_ProbeResponse _63_Union__1__::get_dot11_mgmt_ProbeResponse() const {
    if (idx_ != 19) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_ProbeResponse >(value_);
}

inline
void _63_Union__1__::set_dot11_mgmt_ProbeResponse(const dot11_mgmt_ProbeResponse& v) {
    idx_ = 19;
    value_ = v;
}

inline
dot11_mgmt_ReassocRequest _63_Union__1__::get_dot11_mgmt_ReassocRequest() const {
    if (idx_ != 20) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_ReassocRequest >(value_);
}

inline
void _63_Union__1__::set_dot11_mgmt_ReassocRequest(const dot11_mgmt_ReassocRequest& v) {
    idx_ = 20;
    value_ = v;
}

inline
dot11_mgmt_ReassocResponse _63_Union__1__::get_dot11_mgmt_ReassocResponse() const {
    if (idx_ != 21) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_ReassocResponse >(value_);
}

inline
void _63_Union__1__::set_dot11_mgmt_ReassocResponse(const dot11_mgmt_ReassocResponse& v) {
    idx_ = 21;
    value_ = v;
}

inline
Unsupported _63_Union__2__::get_Unsupported() const {
    if (idx_ != 0) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Unsupported >(value_);
}

inline
void _63_Union__2__::set_Unsupported(const Unsupported& v) {
    idx_ = 0;
    value_ = v;
}

inline
ethernet2_Ethernet2 _63_Union__2__::get_ethernet2_Ethernet2() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<ethernet2_Ethernet2 >(value_);
}

inline
void _63_Union__2__::set_ethernet2_Ethernet2(const ethernet2_Ethernet2& v) {
    idx_ = 1;
    value_ = v;
}

inline
radiotap_Radiotap _63_Union__2__::get_radiotap_Radiotap() const {
    if (idx_ != 2) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<radiotap_Radiotap >(value_);
}

inline
void _63_Union__2__::set_radiotap_Radiotap(const radiotap_Radiotap& v) {
    idx_ = 2;
    value_ = v;
}

inline
Unsupported _63_Union__3__::get_Unsupported() const {
    if (idx_ != 0) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Unsupported >(value_);
}

inline
void _63_Union__3__::set_Unsupported(const Unsupported& v) {
    idx_ = 0;
    value_ = v;
}

inline
boost::array<uint8_t, 6> _63_Union__3__::get_MacAddr() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<boost::array<uint8_t, 6> >(value_);
}

inline
void _63_Union__3__::set_MacAddr(const boost::array<uint8_t, 6>& v) {
    idx_ = 1;
    value_ = v;
}

inline
ethernet2_Ethernet2 _63_Union__3__::get_ethernet2_Ethernet2() const {
    if (idx_ != 2) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<ethernet2_Ethernet2 >(value_);
}

inline
void _63_Union__3__::set_ethernet2_Ethernet2(const ethernet2_Ethernet2& v) {
    idx_ = 2;
    value_ = v;
}

inline
dot11_Header _63_Union__3__::get_dot11_Header() const {
    if (idx_ != 3) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_Header >(value_);
}

inline
void _63_Union__3__::set_dot11_Header(const dot11_Header& v) {
    idx_ = 3;
    value_ = v;
}

inline
dot11_Unsupported _63_Union__3__::get_dot11_Unsupported() const {
    if (idx_ != 4) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_Unsupported >(value_);
}

inline
void _63_Union__3__::set_dot11_Unsupported(const dot11_Unsupported& v) {
    idx_ = 4;
    value_ = v;
}

inline
dot11_ctrl_Ack _63_Union__3__::get_dot11_ctrl_Ack() const {
    if (idx_ != 5) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_Ack >(value_);
}

inline
void _63_Union__3__::set_dot11_ctrl_Ack(const dot11_ctrl_Ack& v) {
    idx_ = 5;
    value_ = v;
}

inline
dot11_ctrl_BlockAck _63_Union__3__::get_dot11_ctrl_BlockAck() const {
    if (idx_ != 6) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_BlockAck >(value_);
}

inline
void _63_Union__3__::set_dot11_ctrl_BlockAck(const dot11_ctrl_BlockAck& v) {
    idx_ = 6;
    value_ = v;
}

inline
dot11_ctrl_BlockAckRequest _63_Union__3__::get_dot11_ctrl_BlockAckRequest() const {
    if (idx_ != 7) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_BlockAckRequest >(value_);
}

inline
void _63_Union__3__::set_dot11_ctrl_BlockAckRequest(const dot11_ctrl_BlockAckRequest& v) {
    idx_ = 7;
    value_ = v;
}

inline
dot11_ctrl_CfEnd _63_Union__3__::get_dot11_ctrl_CfEnd() const {
    if (idx_ != 8) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_CfEnd >(value_);
}

inline
void _63_Union__3__::set_dot11_ctrl_CfEnd(const dot11_ctrl_CfEnd& v) {
    idx_ = 8;
    value_ = v;
}

inline
dot11_ctrl_EndCfAck _63_Union__3__::get_dot11_ctrl_EndCfAck() const {
    if (idx_ != 9) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_EndCfAck >(value_);
}

inline
void _63_Union__3__::set_dot11_ctrl_EndCfAck(const dot11_ctrl_EndCfAck& v) {
    idx_ = 9;
    value_ = v;
}

inline
dot11_ctrl_PsPoll _63_Union__3__::get_dot11_ctrl_PsPoll() const {
    if (idx_ != 10) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_PsPoll >(value_);
}

inline
void _63_Union__3__::set_dot11_ctrl_PsPoll(const dot11_ctrl_PsPoll& v) {
    idx_ = 10;
    value_ = v;
}

inline
dot11_ctrl_Rts _63_Union__3__::get_dot11_ctrl_Rts() const {
    if (idx_ != 11) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_ctrl_Rts >(value_);
}

inline
void _63_Union__3__::set_dot11_ctrl_Rts(const dot11_ctrl_Rts& v) {
    idx_ = 11;
    value_ = v;
}

inline
dot11_data_Header _63_Union__3__::get_dot11_data_Header() const {
    if (idx_ != 12) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_data_Header >(value_);
}

inline
void _63_Union__3__::set_dot11_data_Header(const dot11_data_Header& v) {
    idx_ = 12;
    value_ = v;
}

inline
dot11_data_Data _63_Union__3__::get_dot11_data_Data() const {
    if (idx_ != 13) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_data_Data >(value_);
}

inline
void _63_Union__3__::set_dot11_data_Data(const dot11_data_Data& v) {
    idx_ = 13;
    value_ = v;
}

inline
dot11_data_QosData _63_Union__3__::get_dot11_data_QosData() const {
    if (idx_ != 14) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_data_QosData >(value_);
}

inline
void _63_Union__3__::set_dot11_data_QosData(const dot11_data_QosData& v) {
    idx_ = 14;
    value_ = v;
}

inline
dot11_mgmt_Capability _63_Union__3__::get_dot11_mgmt_Capability() const {
    if (idx_ != 15) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_Capability >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_Capability(const dot11_mgmt_Capability& v) {
    idx_ = 15;
    value_ = v;
}

inline
dot11_mgmt_Header _63_Union__3__::get_dot11_mgmt_Header() const {
    if (idx_ != 16) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_Header >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_Header(const dot11_mgmt_Header& v) {
    idx_ = 16;
    value_ = v;
}

inline
dot11_mgmt_AssocRequest _63_Union__3__::get_dot11_mgmt_AssocRequest() const {
    if (idx_ != 17) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_AssocRequest >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_AssocRequest(const dot11_mgmt_AssocRequest& v) {
    idx_ = 17;
    value_ = v;
}

inline
dot11_mgmt_AssocResponse _63_Union__3__::get_dot11_mgmt_AssocResponse() const {
    if (idx_ != 18) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_AssocResponse >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_AssocResponse(const dot11_mgmt_AssocResponse& v) {
    idx_ = 18;
    value_ = v;
}

inline
dot11_mgmt_Authentication _63_Union__3__::get_dot11_mgmt_Authentication() const {
    if (idx_ != 19) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_Authentication >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_Authentication(const dot11_mgmt_Authentication& v) {
    idx_ = 19;
    value_ = v;
}

inline
dot11_mgmt_Beacon _63_Union__3__::get_dot11_mgmt_Beacon() const {
    if (idx_ != 20) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_Beacon >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_Beacon(const dot11_mgmt_Beacon& v) {
    idx_ = 20;
    value_ = v;
}

inline
dot11_mgmt_Deauthentication _63_Union__3__::get_dot11_mgmt_Deauthentication() const {
    if (idx_ != 21) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_Deauthentication >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_Deauthentication(const dot11_mgmt_Deauthentication& v) {
    idx_ = 21;
    value_ = v;
}

inline
dot11_mgmt_Disassoc _63_Union__3__::get_dot11_mgmt_Disassoc() const {
    if (idx_ != 22) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_Disassoc >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_Disassoc(const dot11_mgmt_Disassoc& v) {
    idx_ = 22;
    value_ = v;
}

inline
dot11_mgmt_ProbeRequest _63_Union__3__::get_dot11_mgmt_ProbeRequest() const {
    if (idx_ != 23) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_ProbeRequest >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_ProbeRequest(const dot11_mgmt_ProbeRequest& v) {
    idx_ = 23;
    value_ = v;
}

inline
dot11_mgmt_ProbeResponse _63_Union__3__::get_dot11_mgmt_ProbeResponse() const {
    if (idx_ != 24) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_ProbeResponse >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_ProbeResponse(const dot11_mgmt_ProbeResponse& v) {
    idx_ = 24;
    value_ = v;
}

inline
dot11_mgmt_ReassocRequest _63_Union__3__::get_dot11_mgmt_ReassocRequest() const {
    if (idx_ != 25) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_ReassocRequest >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_ReassocRequest(const dot11_mgmt_ReassocRequest& v) {
    idx_ = 25;
    value_ = v;
}

inline
dot11_mgmt_ReassocResponse _63_Union__3__::get_dot11_mgmt_ReassocResponse() const {
    if (idx_ != 26) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<dot11_mgmt_ReassocResponse >(value_);
}

inline
void _63_Union__3__::set_dot11_mgmt_ReassocResponse(const dot11_mgmt_ReassocResponse& v) {
    idx_ = 26;
    value_ = v;
}

inline
radiotap_Flag _63_Union__3__::get_radiotap_Flag() const {
    if (idx_ != 27) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<radiotap_Flag >(value_);
}

inline
void _63_Union__3__::set_radiotap_Flag(const radiotap_Flag& v) {
    idx_ = 27;
    value_ = v;
}

inline
radiotap_ChannelType _63_Union__3__::get_radiotap_ChannelType() const {
    if (idx_ != 28) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<radiotap_ChannelType >(value_);
}

inline
void _63_Union__3__::set_radiotap_ChannelType(const radiotap_ChannelType& v) {
    idx_ = 28;
    value_ = v;
}

inline
radiotap_Channel _63_Union__3__::get_radiotap_Channel() const {
    if (idx_ != 29) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<radiotap_Channel >(value_);
}

inline
void _63_Union__3__::set_radiotap_Channel(const radiotap_Channel& v) {
    idx_ = 29;
    value_ = v;
}

inline
radiotap_Radiotap _63_Union__3__::get_radiotap_Radiotap() const {
    if (idx_ != 30) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<radiotap_Radiotap >(value_);
}

inline
void _63_Union__3__::set_radiotap_Radiotap(const radiotap_Radiotap& v) {
    idx_ = 30;
    value_ = v;
}

inline
Pdu _63_Union__3__::get_Pdu() const {
    if (idx_ != 31) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<Pdu >(value_);
}

inline
void _63_Union__3__::set_Pdu(const Pdu& v) {
    idx_ = 31;
    value_ = v;
}

inline _63_Union__0__::_63_Union__0__() : idx_(0) { }
inline _63_Union__1__::_63_Union__1__() : idx_(0) { }
inline _63_Union__2__::_63_Union__2__() : idx_(0), value_(Unsupported()) { }
inline _63_Union__3__::_63_Union__3__() : idx_(0), value_(Unsupported()) { }
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

template<> struct codec_traits<Layer2::ethernet2_Ethernet2> {
    static void encode(Encoder& e, const Layer2::ethernet2_Ethernet2& v) {
        avro::encode(e, v.srcAddr);
        avro::encode(e, v.dstAddr);
        avro::encode(e, v.payloadType);
        avro::encode(e, v.data);
    }
    static void decode(Decoder& d, Layer2::ethernet2_Ethernet2& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.srcAddr);
                    break;
                case 1:
                    avro::decode(d, v.dstAddr);
                    break;
                case 2:
                    avro::decode(d, v.payloadType);
                    break;
                case 3:
                    avro::decode(d, v.data);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.srcAddr);
            avro::decode(d, v.dstAddr);
            avro::decode(d, v.payloadType);
            avro::decode(d, v.data);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_Header> {
    static void encode(Encoder& e, const Layer2::dot11_Header& v) {
        avro::encode(e, v.toDs);
        avro::encode(e, v.fromDs);
        avro::encode(e, v.moreFrag);
        avro::encode(e, v.retry);
        avro::encode(e, v.powerMgmt);
        avro::encode(e, v.wep);
        avro::encode(e, v.order);
        avro::encode(e, v.durationId);
        avro::encode(e, v.addr1);
    }
    static void decode(Decoder& d, Layer2::dot11_Header& v) {
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
                    avro::decode(d, v.retry);
                    break;
                case 4:
                    avro::decode(d, v.powerMgmt);
                    break;
                case 5:
                    avro::decode(d, v.wep);
                    break;
                case 6:
                    avro::decode(d, v.order);
                    break;
                case 7:
                    avro::decode(d, v.durationId);
                    break;
                case 8:
                    avro::decode(d, v.addr1);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.toDs);
            avro::decode(d, v.fromDs);
            avro::decode(d, v.moreFrag);
            avro::decode(d, v.retry);
            avro::decode(d, v.powerMgmt);
            avro::decode(d, v.wep);
            avro::decode(d, v.order);
            avro::decode(d, v.durationId);
            avro::decode(d, v.addr1);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_Unsupported> {
    static void encode(Encoder& e, const Layer2::dot11_Unsupported& v) {
        avro::encode(e, v.type);
        avro::encode(e, v.subtype);
    }
    static void decode(Decoder& d, Layer2::dot11_Unsupported& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.type);
                    break;
                case 1:
                    avro::decode(d, v.subtype);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.type);
            avro::decode(d, v.subtype);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_ctrl_Ack> {
    static void encode(Encoder& e, const Layer2::dot11_ctrl_Ack& v) {
        avro::encode(e, v.header);
    }
    static void decode(Decoder& d, Layer2::dot11_ctrl_Ack& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_ctrl_BlockAck> {
    static void encode(Encoder& e, const Layer2::dot11_ctrl_BlockAck& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.barControl);
        avro::encode(e, v.startSeq);
        avro::encode(e, v.fragNum);
    }
    static void decode(Decoder& d, Layer2::dot11_ctrl_BlockAck& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.barControl);
                    break;
                case 2:
                    avro::decode(d, v.startSeq);
                    break;
                case 3:
                    avro::decode(d, v.fragNum);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.barControl);
            avro::decode(d, v.startSeq);
            avro::decode(d, v.fragNum);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_ctrl_BlockAckRequest> {
    static void encode(Encoder& e, const Layer2::dot11_ctrl_BlockAckRequest& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.barControl);
        avro::encode(e, v.startSeq);
        avro::encode(e, v.fragNum);
    }
    static void decode(Decoder& d, Layer2::dot11_ctrl_BlockAckRequest& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.barControl);
                    break;
                case 2:
                    avro::decode(d, v.startSeq);
                    break;
                case 3:
                    avro::decode(d, v.fragNum);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.barControl);
            avro::decode(d, v.startSeq);
            avro::decode(d, v.fragNum);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_ctrl_CfEnd> {
    static void encode(Encoder& e, const Layer2::dot11_ctrl_CfEnd& v) {
        avro::encode(e, v.header);
    }
    static void decode(Decoder& d, Layer2::dot11_ctrl_CfEnd& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_ctrl_EndCfAck> {
    static void encode(Encoder& e, const Layer2::dot11_ctrl_EndCfAck& v) {
        avro::encode(e, v.header);
    }
    static void decode(Decoder& d, Layer2::dot11_ctrl_EndCfAck& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_ctrl_PsPoll> {
    static void encode(Encoder& e, const Layer2::dot11_ctrl_PsPoll& v) {
        avro::encode(e, v.header);
    }
    static void decode(Decoder& d, Layer2::dot11_ctrl_PsPoll& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_ctrl_Rts> {
    static void encode(Encoder& e, const Layer2::dot11_ctrl_Rts& v) {
        avro::encode(e, v.header);
    }
    static void decode(Decoder& d, Layer2::dot11_ctrl_Rts& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_data_Header> {
    static void encode(Encoder& e, const Layer2::dot11_data_Header& v) {
        avro::encode(e, v.addr2);
        avro::encode(e, v.addr3);
        avro::encode(e, v.addr4);
        avro::encode(e, v.fragNum);
        avro::encode(e, v.seqNum);
    }
    static void decode(Decoder& d, Layer2::dot11_data_Header& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.addr2);
                    break;
                case 1:
                    avro::decode(d, v.addr3);
                    break;
                case 2:
                    avro::decode(d, v.addr4);
                    break;
                case 3:
                    avro::decode(d, v.fragNum);
                    break;
                case 4:
                    avro::decode(d, v.seqNum);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.addr2);
            avro::decode(d, v.addr3);
            avro::decode(d, v.addr4);
            avro::decode(d, v.fragNum);
            avro::decode(d, v.seqNum);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_data_Data> {
    static void encode(Encoder& e, const Layer2::dot11_data_Data& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.dataHeader);
    }
    static void decode(Decoder& d, Layer2::dot11_data_Data& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.dataHeader);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.dataHeader);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_data_QosData> {
    static void encode(Encoder& e, const Layer2::dot11_data_QosData& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.dataHeader);
        avro::encode(e, v.qosControl);
    }
    static void decode(Decoder& d, Layer2::dot11_data_QosData& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.dataHeader);
                    break;
                case 2:
                    avro::decode(d, v.qosControl);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.dataHeader);
            avro::decode(d, v.qosControl);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_Capability> {
    static void encode(Encoder& e, Layer2::dot11_mgmt_Capability v) {
		if (v < Layer2::ESS || v > Layer2::IMMEDIATE_BLOCK_ACK)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for Layer2::dot11_mgmt_Capability and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_Capability& v) {
		size_t index = d.decodeEnum();
		if (index < Layer2::ESS || index > Layer2::IMMEDIATE_BLOCK_ACK)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for Layer2::dot11_mgmt_Capability and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<Layer2::dot11_mgmt_Capability>(index);
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_Header> {
    static void encode(Encoder& e, const Layer2::dot11_mgmt_Header& v) {
        avro::encode(e, v.addr2);
        avro::encode(e, v.addr3);
        avro::encode(e, v.addr4);
        avro::encode(e, v.fragNum);
        avro::encode(e, v.seqNum);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_Header& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.addr2);
                    break;
                case 1:
                    avro::decode(d, v.addr3);
                    break;
                case 2:
                    avro::decode(d, v.addr4);
                    break;
                case 3:
                    avro::decode(d, v.fragNum);
                    break;
                case 4:
                    avro::decode(d, v.seqNum);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.addr2);
            avro::decode(d, v.addr3);
            avro::decode(d, v.addr4);
            avro::decode(d, v.fragNum);
            avro::decode(d, v.seqNum);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_AssocRequest> {
    static void encode(Encoder& e, const Layer2::dot11_mgmt_AssocRequest& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.mgmtHeader);
        avro::encode(e, v.capabilities);
        avro::encode(e, v.listenInterval);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_AssocRequest& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.mgmtHeader);
                    break;
                case 2:
                    avro::decode(d, v.capabilities);
                    break;
                case 3:
                    avro::decode(d, v.listenInterval);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.mgmtHeader);
            avro::decode(d, v.capabilities);
            avro::decode(d, v.listenInterval);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_AssocResponse> {
    static void encode(Encoder& e, const Layer2::dot11_mgmt_AssocResponse& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.mgmtHeader);
        avro::encode(e, v.capabilities);
        avro::encode(e, v.statusCode);
        avro::encode(e, v.aid);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_AssocResponse& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.mgmtHeader);
                    break;
                case 2:
                    avro::decode(d, v.capabilities);
                    break;
                case 3:
                    avro::decode(d, v.statusCode);
                    break;
                case 4:
                    avro::decode(d, v.aid);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.mgmtHeader);
            avro::decode(d, v.capabilities);
            avro::decode(d, v.statusCode);
            avro::decode(d, v.aid);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_Authentication> {
    static void encode(Encoder& e, const Layer2::dot11_mgmt_Authentication& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.mgmtHeader);
        avro::encode(e, v.authAlgorithm);
        avro::encode(e, v.authSeqNumber);
        avro::encode(e, v.statusCode);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_Authentication& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.mgmtHeader);
                    break;
                case 2:
                    avro::decode(d, v.authAlgorithm);
                    break;
                case 3:
                    avro::decode(d, v.authSeqNumber);
                    break;
                case 4:
                    avro::decode(d, v.statusCode);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.mgmtHeader);
            avro::decode(d, v.authAlgorithm);
            avro::decode(d, v.authSeqNumber);
            avro::decode(d, v.statusCode);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_Beacon> {
    static void encode(Encoder& e, const Layer2::dot11_mgmt_Beacon& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.mgmtHeader);
        avro::encode(e, v.timestamp);
        avro::encode(e, v.interval);
        avro::encode(e, v.capabilities);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_Beacon& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.mgmtHeader);
                    break;
                case 2:
                    avro::decode(d, v.timestamp);
                    break;
                case 3:
                    avro::decode(d, v.interval);
                    break;
                case 4:
                    avro::decode(d, v.capabilities);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.mgmtHeader);
            avro::decode(d, v.timestamp);
            avro::decode(d, v.interval);
            avro::decode(d, v.capabilities);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_Deauthentication> {
    static void encode(Encoder& e, const Layer2::dot11_mgmt_Deauthentication& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.mgmtHeader);
        avro::encode(e, v.reasonCode);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_Deauthentication& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.mgmtHeader);
                    break;
                case 2:
                    avro::decode(d, v.reasonCode);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.mgmtHeader);
            avro::decode(d, v.reasonCode);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_Disassoc> {
    static void encode(Encoder& e, const Layer2::dot11_mgmt_Disassoc& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.mgmtHeader);
        avro::encode(e, v.reasonCode);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_Disassoc& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.mgmtHeader);
                    break;
                case 2:
                    avro::decode(d, v.reasonCode);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.mgmtHeader);
            avro::decode(d, v.reasonCode);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_ProbeRequest> {
    static void encode(Encoder& e, const Layer2::dot11_mgmt_ProbeRequest& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.mgmtHeader);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_ProbeRequest& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.mgmtHeader);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.mgmtHeader);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_ProbeResponse> {
    static void encode(Encoder& e, const Layer2::dot11_mgmt_ProbeResponse& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.mgmtHeader);
        avro::encode(e, v.timestamp);
        avro::encode(e, v.interval);
        avro::encode(e, v.capabilities);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_ProbeResponse& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.mgmtHeader);
                    break;
                case 2:
                    avro::decode(d, v.timestamp);
                    break;
                case 3:
                    avro::decode(d, v.interval);
                    break;
                case 4:
                    avro::decode(d, v.capabilities);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.mgmtHeader);
            avro::decode(d, v.timestamp);
            avro::decode(d, v.interval);
            avro::decode(d, v.capabilities);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_ReassocRequest> {
    static void encode(Encoder& e, const Layer2::dot11_mgmt_ReassocRequest& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.mgmtHeader);
        avro::encode(e, v.capabilities);
        avro::encode(e, v.listenInterval);
        avro::encode(e, v.currentAp);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_ReassocRequest& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.mgmtHeader);
                    break;
                case 2:
                    avro::decode(d, v.capabilities);
                    break;
                case 3:
                    avro::decode(d, v.listenInterval);
                    break;
                case 4:
                    avro::decode(d, v.currentAp);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.mgmtHeader);
            avro::decode(d, v.capabilities);
            avro::decode(d, v.listenInterval);
            avro::decode(d, v.currentAp);
        }
    }
};

template<> struct codec_traits<Layer2::dot11_mgmt_ReassocResponse> {
    static void encode(Encoder& e, const Layer2::dot11_mgmt_ReassocResponse& v) {
        avro::encode(e, v.header);
        avro::encode(e, v.mgmtHeader);
        avro::encode(e, v.capabilities);
        avro::encode(e, v.statusCode);
        avro::encode(e, v.aid);
    }
    static void decode(Decoder& d, Layer2::dot11_mgmt_ReassocResponse& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.header);
                    break;
                case 1:
                    avro::decode(d, v.mgmtHeader);
                    break;
                case 2:
                    avro::decode(d, v.capabilities);
                    break;
                case 3:
                    avro::decode(d, v.statusCode);
                    break;
                case 4:
                    avro::decode(d, v.aid);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.header);
            avro::decode(d, v.mgmtHeader);
            avro::decode(d, v.capabilities);
            avro::decode(d, v.statusCode);
            avro::decode(d, v.aid);
        }
    }
};

template<> struct codec_traits<Layer2::radiotap_Flag> {
    static void encode(Encoder& e, Layer2::radiotap_Flag v) {
		if (v < Layer2::CFP || v > Layer2::SHORT_GI)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for Layer2::radiotap_Flag and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, Layer2::radiotap_Flag& v) {
		size_t index = d.decodeEnum();
		if (index < Layer2::CFP || index > Layer2::SHORT_GI)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for Layer2::radiotap_Flag and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<Layer2::radiotap_Flag>(index);
    }
};

template<> struct codec_traits<Layer2::radiotap_ChannelType> {
    static void encode(Encoder& e, Layer2::radiotap_ChannelType v) {
		if (v < Layer2::TURBO || v > Layer2::GFSK)
		{
			std::ostringstream error;
			error << "enum value " << v << " is out of bound for Layer2::radiotap_ChannelType and cannot be encoded";
			throw avro::Exception(error.str());
		}
        e.encodeEnum(v);
    }
    static void decode(Decoder& d, Layer2::radiotap_ChannelType& v) {
		size_t index = d.decodeEnum();
		if (index < Layer2::TURBO || index > Layer2::GFSK)
		{
			std::ostringstream error;
			error << "enum value " << index << " is out of bound for Layer2::radiotap_ChannelType and cannot be decoded";
			throw avro::Exception(error.str());
		}
        v = static_cast<Layer2::radiotap_ChannelType>(index);
    }
};

template<> struct codec_traits<Layer2::radiotap_Channel> {
    static void encode(Encoder& e, const Layer2::radiotap_Channel& v) {
        avro::encode(e, v.freq);
        avro::encode(e, v.type);
    }
    static void decode(Decoder& d, Layer2::radiotap_Channel& v) {
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

template<> struct codec_traits<Layer2::_63_Union__0__> {
    static void encode(Encoder& e, Layer2::_63_Union__0__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            e.encodeNull();
            break;
        case 1:
            avro::encode(e, v.get_radiotap_Channel());
            break;
        }
    }
    static void decode(Decoder& d, Layer2::_63_Union__0__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 2) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            d.decodeNull();
            v.set_null();
            break;
        case 1:
            {
                Layer2::radiotap_Channel vv;
                avro::decode(d, vv);
                v.set_radiotap_Channel(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<Layer2::_63_Union__1__> {
    static void encode(Encoder& e, Layer2::_63_Union__1__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            e.encodeNull();
            break;
        case 1:
            avro::encode(e, v.get_Unsupported());
            break;
        case 2:
            avro::encode(e, v.get_dot11_Unsupported());
            break;
        case 3:
            avro::encode(e, v.get_dot11_ctrl_Ack());
            break;
        case 4:
            avro::encode(e, v.get_dot11_ctrl_BlockAck());
            break;
        case 5:
            avro::encode(e, v.get_dot11_ctrl_BlockAckRequest());
            break;
        case 6:
            avro::encode(e, v.get_dot11_ctrl_CfEnd());
            break;
        case 7:
            avro::encode(e, v.get_dot11_ctrl_EndCfAck());
            break;
        case 8:
            avro::encode(e, v.get_dot11_ctrl_PsPoll());
            break;
        case 9:
            avro::encode(e, v.get_dot11_ctrl_Rts());
            break;
        case 10:
            avro::encode(e, v.get_dot11_data_Data());
            break;
        case 11:
            avro::encode(e, v.get_dot11_data_QosData());
            break;
        case 12:
            avro::encode(e, v.get_dot11_mgmt_AssocRequest());
            break;
        case 13:
            avro::encode(e, v.get_dot11_mgmt_AssocResponse());
            break;
        case 14:
            avro::encode(e, v.get_dot11_mgmt_Authentication());
            break;
        case 15:
            avro::encode(e, v.get_dot11_mgmt_Beacon());
            break;
        case 16:
            avro::encode(e, v.get_dot11_mgmt_Deauthentication());
            break;
        case 17:
            avro::encode(e, v.get_dot11_mgmt_Disassoc());
            break;
        case 18:
            avro::encode(e, v.get_dot11_mgmt_ProbeRequest());
            break;
        case 19:
            avro::encode(e, v.get_dot11_mgmt_ProbeResponse());
            break;
        case 20:
            avro::encode(e, v.get_dot11_mgmt_ReassocRequest());
            break;
        case 21:
            avro::encode(e, v.get_dot11_mgmt_ReassocResponse());
            break;
        }
    }
    static void decode(Decoder& d, Layer2::_63_Union__1__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 22) { throw avro::Exception("Union index too big"); }
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
                Layer2::dot11_Unsupported vv;
                avro::decode(d, vv);
                v.set_dot11_Unsupported(vv);
            }
            break;
        case 3:
            {
                Layer2::dot11_ctrl_Ack vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_Ack(vv);
            }
            break;
        case 4:
            {
                Layer2::dot11_ctrl_BlockAck vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_BlockAck(vv);
            }
            break;
        case 5:
            {
                Layer2::dot11_ctrl_BlockAckRequest vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_BlockAckRequest(vv);
            }
            break;
        case 6:
            {
                Layer2::dot11_ctrl_CfEnd vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_CfEnd(vv);
            }
            break;
        case 7:
            {
                Layer2::dot11_ctrl_EndCfAck vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_EndCfAck(vv);
            }
            break;
        case 8:
            {
                Layer2::dot11_ctrl_PsPoll vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_PsPoll(vv);
            }
            break;
        case 9:
            {
                Layer2::dot11_ctrl_Rts vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_Rts(vv);
            }
            break;
        case 10:
            {
                Layer2::dot11_data_Data vv;
                avro::decode(d, vv);
                v.set_dot11_data_Data(vv);
            }
            break;
        case 11:
            {
                Layer2::dot11_data_QosData vv;
                avro::decode(d, vv);
                v.set_dot11_data_QosData(vv);
            }
            break;
        case 12:
            {
                Layer2::dot11_mgmt_AssocRequest vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_AssocRequest(vv);
            }
            break;
        case 13:
            {
                Layer2::dot11_mgmt_AssocResponse vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_AssocResponse(vv);
            }
            break;
        case 14:
            {
                Layer2::dot11_mgmt_Authentication vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_Authentication(vv);
            }
            break;
        case 15:
            {
                Layer2::dot11_mgmt_Beacon vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_Beacon(vv);
            }
            break;
        case 16:
            {
                Layer2::dot11_mgmt_Deauthentication vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_Deauthentication(vv);
            }
            break;
        case 17:
            {
                Layer2::dot11_mgmt_Disassoc vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_Disassoc(vv);
            }
            break;
        case 18:
            {
                Layer2::dot11_mgmt_ProbeRequest vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_ProbeRequest(vv);
            }
            break;
        case 19:
            {
                Layer2::dot11_mgmt_ProbeResponse vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_ProbeResponse(vv);
            }
            break;
        case 20:
            {
                Layer2::dot11_mgmt_ReassocRequest vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_ReassocRequest(vv);
            }
            break;
        case 21:
            {
                Layer2::dot11_mgmt_ReassocResponse vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_ReassocResponse(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<Layer2::radiotap_Radiotap> {
    static void encode(Encoder& e, const Layer2::radiotap_Radiotap& v) {
        avro::encode(e, v.tsft);
        avro::encode(e, v.flags);
        avro::encode(e, v.rate);
        avro::encode(e, v.channel);
        avro::encode(e, v.pdu);
    }
    static void decode(Decoder& d, Layer2::radiotap_Radiotap& v) {
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

template<> struct codec_traits<Layer2::_63_Union__2__> {
    static void encode(Encoder& e, Layer2::_63_Union__2__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            avro::encode(e, v.get_Unsupported());
            break;
        case 1:
            avro::encode(e, v.get_ethernet2_Ethernet2());
            break;
        case 2:
            avro::encode(e, v.get_radiotap_Radiotap());
            break;
        }
    }
    static void decode(Decoder& d, Layer2::_63_Union__2__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 3) { throw avro::Exception("Union index too big"); }
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
                Layer2::ethernet2_Ethernet2 vv;
                avro::decode(d, vv);
                v.set_ethernet2_Ethernet2(vv);
            }
            break;
        case 2:
            {
                Layer2::radiotap_Radiotap vv;
                avro::decode(d, vv);
                v.set_radiotap_Radiotap(vv);
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

template<> struct codec_traits<Layer2::_63_Union__3__> {
    static void encode(Encoder& e, Layer2::_63_Union__3__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            avro::encode(e, v.get_Unsupported());
            break;
        case 1:
            avro::encode(e, v.get_MacAddr());
            break;
        case 2:
            avro::encode(e, v.get_ethernet2_Ethernet2());
            break;
        case 3:
            avro::encode(e, v.get_dot11_Header());
            break;
        case 4:
            avro::encode(e, v.get_dot11_Unsupported());
            break;
        case 5:
            avro::encode(e, v.get_dot11_ctrl_Ack());
            break;
        case 6:
            avro::encode(e, v.get_dot11_ctrl_BlockAck());
            break;
        case 7:
            avro::encode(e, v.get_dot11_ctrl_BlockAckRequest());
            break;
        case 8:
            avro::encode(e, v.get_dot11_ctrl_CfEnd());
            break;
        case 9:
            avro::encode(e, v.get_dot11_ctrl_EndCfAck());
            break;
        case 10:
            avro::encode(e, v.get_dot11_ctrl_PsPoll());
            break;
        case 11:
            avro::encode(e, v.get_dot11_ctrl_Rts());
            break;
        case 12:
            avro::encode(e, v.get_dot11_data_Header());
            break;
        case 13:
            avro::encode(e, v.get_dot11_data_Data());
            break;
        case 14:
            avro::encode(e, v.get_dot11_data_QosData());
            break;
        case 15:
            avro::encode(e, v.get_dot11_mgmt_Capability());
            break;
        case 16:
            avro::encode(e, v.get_dot11_mgmt_Header());
            break;
        case 17:
            avro::encode(e, v.get_dot11_mgmt_AssocRequest());
            break;
        case 18:
            avro::encode(e, v.get_dot11_mgmt_AssocResponse());
            break;
        case 19:
            avro::encode(e, v.get_dot11_mgmt_Authentication());
            break;
        case 20:
            avro::encode(e, v.get_dot11_mgmt_Beacon());
            break;
        case 21:
            avro::encode(e, v.get_dot11_mgmt_Deauthentication());
            break;
        case 22:
            avro::encode(e, v.get_dot11_mgmt_Disassoc());
            break;
        case 23:
            avro::encode(e, v.get_dot11_mgmt_ProbeRequest());
            break;
        case 24:
            avro::encode(e, v.get_dot11_mgmt_ProbeResponse());
            break;
        case 25:
            avro::encode(e, v.get_dot11_mgmt_ReassocRequest());
            break;
        case 26:
            avro::encode(e, v.get_dot11_mgmt_ReassocResponse());
            break;
        case 27:
            avro::encode(e, v.get_radiotap_Flag());
            break;
        case 28:
            avro::encode(e, v.get_radiotap_ChannelType());
            break;
        case 29:
            avro::encode(e, v.get_radiotap_Channel());
            break;
        case 30:
            avro::encode(e, v.get_radiotap_Radiotap());
            break;
        case 31:
            avro::encode(e, v.get_Pdu());
            break;
        }
    }
    static void decode(Decoder& d, Layer2::_63_Union__3__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 32) { throw avro::Exception("Union index too big"); }
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
                Layer2::ethernet2_Ethernet2 vv;
                avro::decode(d, vv);
                v.set_ethernet2_Ethernet2(vv);
            }
            break;
        case 3:
            {
                Layer2::dot11_Header vv;
                avro::decode(d, vv);
                v.set_dot11_Header(vv);
            }
            break;
        case 4:
            {
                Layer2::dot11_Unsupported vv;
                avro::decode(d, vv);
                v.set_dot11_Unsupported(vv);
            }
            break;
        case 5:
            {
                Layer2::dot11_ctrl_Ack vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_Ack(vv);
            }
            break;
        case 6:
            {
                Layer2::dot11_ctrl_BlockAck vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_BlockAck(vv);
            }
            break;
        case 7:
            {
                Layer2::dot11_ctrl_BlockAckRequest vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_BlockAckRequest(vv);
            }
            break;
        case 8:
            {
                Layer2::dot11_ctrl_CfEnd vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_CfEnd(vv);
            }
            break;
        case 9:
            {
                Layer2::dot11_ctrl_EndCfAck vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_EndCfAck(vv);
            }
            break;
        case 10:
            {
                Layer2::dot11_ctrl_PsPoll vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_PsPoll(vv);
            }
            break;
        case 11:
            {
                Layer2::dot11_ctrl_Rts vv;
                avro::decode(d, vv);
                v.set_dot11_ctrl_Rts(vv);
            }
            break;
        case 12:
            {
                Layer2::dot11_data_Header vv;
                avro::decode(d, vv);
                v.set_dot11_data_Header(vv);
            }
            break;
        case 13:
            {
                Layer2::dot11_data_Data vv;
                avro::decode(d, vv);
                v.set_dot11_data_Data(vv);
            }
            break;
        case 14:
            {
                Layer2::dot11_data_QosData vv;
                avro::decode(d, vv);
                v.set_dot11_data_QosData(vv);
            }
            break;
        case 15:
            {
                Layer2::dot11_mgmt_Capability vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_Capability(vv);
            }
            break;
        case 16:
            {
                Layer2::dot11_mgmt_Header vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_Header(vv);
            }
            break;
        case 17:
            {
                Layer2::dot11_mgmt_AssocRequest vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_AssocRequest(vv);
            }
            break;
        case 18:
            {
                Layer2::dot11_mgmt_AssocResponse vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_AssocResponse(vv);
            }
            break;
        case 19:
            {
                Layer2::dot11_mgmt_Authentication vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_Authentication(vv);
            }
            break;
        case 20:
            {
                Layer2::dot11_mgmt_Beacon vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_Beacon(vv);
            }
            break;
        case 21:
            {
                Layer2::dot11_mgmt_Deauthentication vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_Deauthentication(vv);
            }
            break;
        case 22:
            {
                Layer2::dot11_mgmt_Disassoc vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_Disassoc(vv);
            }
            break;
        case 23:
            {
                Layer2::dot11_mgmt_ProbeRequest vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_ProbeRequest(vv);
            }
            break;
        case 24:
            {
                Layer2::dot11_mgmt_ProbeResponse vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_ProbeResponse(vv);
            }
            break;
        case 25:
            {
                Layer2::dot11_mgmt_ReassocRequest vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_ReassocRequest(vv);
            }
            break;
        case 26:
            {
                Layer2::dot11_mgmt_ReassocResponse vv;
                avro::decode(d, vv);
                v.set_dot11_mgmt_ReassocResponse(vv);
            }
            break;
        case 27:
            {
                Layer2::radiotap_Flag vv;
                avro::decode(d, vv);
                v.set_radiotap_Flag(vv);
            }
            break;
        case 28:
            {
                Layer2::radiotap_ChannelType vv;
                avro::decode(d, vv);
                v.set_radiotap_ChannelType(vv);
            }
            break;
        case 29:
            {
                Layer2::radiotap_Channel vv;
                avro::decode(d, vv);
                v.set_radiotap_Channel(vv);
            }
            break;
        case 30:
            {
                Layer2::radiotap_Radiotap vv;
                avro::decode(d, vv);
                v.set_radiotap_Radiotap(vv);
            }
            break;
        case 31:
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
