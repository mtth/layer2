/*
 * Copyright (c) 2001
 *  Fortress Technologies, Inc.  All rights reserved.
 *      Charlie Lenahan (clenahan@fortresstech.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "cpack.hpp"
#include "extract.hpp"
#include "radiotap.hpp"
// #include <string.h>

using v8::Boolean;
using v8::FunctionTemplate;
using v8::Integer;
using v8::Handle;
using v8::Local;
using v8::Object;
using v8::String;

struct ieee80211_radiotap_header {
  uint8_t it_version;
  uint8_t it_pad;
  uint16_t it_len;
  uint32_t  it_present;
};

enum ieee80211_radiotap_type {
  RTAP_TSFT = 0,
  RTAP_FLAGS = 1,
  RTAP_RATE = 2,
  RTAP_CHANNEL = 3,
  RTAP_FHSS = 4,
  RTAP_DBM_ANTSIGNAL = 5,
  RTAP_DBM_ANTNOISE = 6,
  RTAP_LOCK_QUALITY = 7,
  RTAP_TX_ATTENUATION = 8,
  RTAP_DB_TX_ATTENUATION = 9,
  RTAP_DBM_TX_POWER = 10,
  RTAP_ANTENNA = 11,
  RTAP_DB_ANTSIGNAL = 12,
  RTAP_DB_ANTNOISE = 13,
  RTAP_RX_FLAGS = 14,
  /* NB: gap for netbsd definitions */
  RTAP_XCHANNEL = 18,
  RTAP_MCS = 19,
  RTAP_NAMESPACE = 29,
  RTAP_VENDOR_NAMESPACE = 30,
  RTAP_EXT = 31
};

struct radiotap_state {
  uint32_t present;
  uint8_t rate;
};

/* channel attributes */
#define CHAN_TURBO  0x00010 /* Turbo channel */
#define CHAN_CCK  0x00020 /* CCK channel */
#define CHAN_OFDM 0x00040 /* OFDM channel */
#define CHAN_2GHZ 0x00080 /* 2 GHz spectrum channel. */
#define CHAN_5GHZ 0x00100 /* 5 GHz spectrum channel */
#define CHAN_PASSIVE  0x00200 /* Only passive scan allowed */
#define CHAN_DYN  0x00400 /* Dynamic CCK-OFDM channel */
#define CHAN_GFSK 0x00800 /* GFSK channel (FHSS PHY) */
#define CHAN_GSM  0x01000 /* 900 MHz spectrum channel */
#define CHAN_STURBO 0x02000 /* 11a static turbo channel only */
#define CHAN_HALF 0x04000 /* Half rate channel */
#define CHAN_QUARTER  0x08000 /* Quarter rate channel */
#define CHAN_HT20 0x10000 /* HT 20 channel */
#define CHAN_HT40U  0x20000 /* HT 40 channel w/ ext above */
#define CHAN_HT40D  0x40000 /* HT 40 channel w/ ext below */

/* Useful combinations of channel characteristics, borrowed from Ethereal */
#define CHAN_A (CHAN_5GHZ | CHAN_OFDM)
#define CHAN_B (CHAN_2GHZ | CHAN_CCK)
#define CHAN_G (CHAN_2GHZ | CHAN_DYN)
#define CHAN_TA (CHAN_5GHZ | CHAN_OFDM | CHAN_TURBO)
#define CHAN_TG (CHAN_2GHZ | CHAN_DYN  | CHAN_TURBO)

/* For RTAP_FLAGS */
#define RTAP_F_CFP  0x01  /* sent/received during CFP */
#define RTAP_F_SHORTPRE 0x02  /* sent/received with short preamble */
#define RTAP_F_WEP  0x04  /* sent/received with WEP encryption */
#define RTAP_F_FRAG 0x08  /* sent/received with fragmentation */
#define RTAP_F_FCS  0x10  /* frame includes FCS */
#define RTAP_F_DATAPAD  0x20  /* frame has padding */
#define RTAP_F_BADFCS 0x40  /* does not pass FCS check */

/* For RTAP_RX_FLAGS */
#define RTAP_F_RX_BADFCS  0x0001  /* frame failed crc check */
#define RTAP_F_RX_PLCP_CRC  0x0002  /* frame failed PLCP CRC check */

/* For RTAP_MCS known */
#define RTAP_MCS_BANDWIDTH_KNOWN    0x01
#define RTAP_MCS_MCS_INDEX_KNOWN    0x02  /* MCS index field */
#define RTAP_MCS_GUARD_INTERVAL_KNOWN 0x04
#define RTAP_MCS_HT_FORMAT_KNOWN    0x08
#define RTAP_MCS_FEC_TYPE_KNOWN   0x10
#define RTAP_MCS_STBC_KNOWN   0x20

/* For RTAP_MCS flags */
#define RTAP_MCS_BANDWIDTH_MASK 0x03
#define RTAP_MCS_BANDWIDTH_20 0
#define RTAP_MCS_BANDWIDTH_40 1
#define RTAP_MCS_BANDWIDTH_20L  2
#define RTAP_MCS_BANDWIDTH_20U  3
#define RTAP_MCS_SHORT_GI   0x04 /* short guard interval */
#define RTAP_MCS_HT_GREENFIELD  0x08
#define RTAP_MCS_FEC_LDPC   0x10
#define RTAP_MCS_STBC_MASK  0x60
#define RTAP_MCS_STBC_1 1
#define RTAP_MCS_STBC_2 2
#define RTAP_MCS_STBC_3 3
#define RTAP_MCS_STBC_SHIFT 5

#define MAX_MCS_INDEX 76

static const float ieee80211_float_htrates[MAX_MCS_INDEX+1][2][2] = {
  /* MCS  0  */
  { /* 20 Mhz */ {    6.5,    /* SGI */    7.2, },
    /* 40 Mhz */ {   13.5,    /* SGI */   15.0, },
  },

  /* MCS  1  */
  { /* 20 Mhz */ {   13.0,    /* SGI */   14.4, },
    /* 40 Mhz */ {   27.0,    /* SGI */   30.0, },
  },

  /* MCS  2  */
  { /* 20 Mhz */ {   19.5,    /* SGI */   21.7, },
    /* 40 Mhz */ {   40.5,    /* SGI */   45.0, },
  },

  /* MCS  3  */
  { /* 20 Mhz */ {   26.0,    /* SGI */   28.9, },
    /* 40 Mhz */ {   54.0,    /* SGI */   60.0, },
  },

  /* MCS  4  */
  { /* 20 Mhz */ {   39.0,    /* SGI */   43.3, },
    /* 40 Mhz */ {   81.0,    /* SGI */   90.0, },
  },

  /* MCS  5  */
  { /* 20 Mhz */ {   52.0,    /* SGI */   57.8, },
    /* 40 Mhz */ {  108.0,    /* SGI */  120.0, },
  },

  /* MCS  6  */
  { /* 20 Mhz */ {   58.5,    /* SGI */   65.0, },
    /* 40 Mhz */ {  121.5,    /* SGI */  135.0, },
  },

  /* MCS  7  */
  { /* 20 Mhz */ {   65.0,    /* SGI */   72.2, },
    /* 40 Mhz */ {   135.0,   /* SGI */  150.0, },
  },

  /* MCS  8  */
  { /* 20 Mhz */ {   13.0,    /* SGI */   14.4, },
    /* 40 Mhz */ {   27.0,    /* SGI */   30.0, },
  },

  /* MCS  9  */
  { /* 20 Mhz */ {   26.0,    /* SGI */   28.9, },
    /* 40 Mhz */ {   54.0,    /* SGI */   60.0, },
  },

  /* MCS 10  */
  { /* 20 Mhz */ {   39.0,    /* SGI */   43.3, },
    /* 40 Mhz */ {   81.0,    /* SGI */   90.0, },
  },

  /* MCS 11  */
  { /* 20 Mhz */ {   52.0,    /* SGI */   57.8, },
    /* 40 Mhz */ {  108.0,    /* SGI */  120.0, },
  },

  /* MCS 12  */
  { /* 20 Mhz */ {   78.0,    /* SGI */   86.7, },
    /* 40 Mhz */ {  162.0,    /* SGI */  180.0, },
  },

  /* MCS 13  */
  { /* 20 Mhz */ {  104.0,    /* SGI */  115.6, },
    /* 40 Mhz */ {  216.0,    /* SGI */  240.0, },
  },

  /* MCS 14  */
  { /* 20 Mhz */ {  117.0,    /* SGI */  130.0, },
    /* 40 Mhz */ {  243.0,    /* SGI */  270.0, },
  },

  /* MCS 15  */
  { /* 20 Mhz */ {  130.0,    /* SGI */  144.4, },
    /* 40 Mhz */ {  270.0,    /* SGI */  300.0, },
  },

  /* MCS 16  */
  { /* 20 Mhz */ {   19.5,    /* SGI */   21.7, },
    /* 40 Mhz */ {   40.5,    /* SGI */   45.0, },
  },

  /* MCS 17  */
  { /* 20 Mhz */ {   39.0,    /* SGI */   43.3, },
    /* 40 Mhz */ {   81.0,    /* SGI */   90.0, },
  },

  /* MCS 18  */
  { /* 20 Mhz */ {   58.5,    /* SGI */   65.0, },
    /* 40 Mhz */ {  121.5,    /* SGI */  135.0, },
  },

  /* MCS 19  */
  { /* 20 Mhz */ {   78.0,    /* SGI */   86.7, },
    /* 40 Mhz */ {  162.0,    /* SGI */  180.0, },
  },

  /* MCS 20  */
  { /* 20 Mhz */ {  117.0,    /* SGI */  130.0, },
    /* 40 Mhz */ {  243.0,    /* SGI */  270.0, },
  },

  /* MCS 21  */
  { /* 20 Mhz */ {  156.0,    /* SGI */  173.3, },
    /* 40 Mhz */ {  324.0,    /* SGI */  360.0, },
  },

  /* MCS 22  */
  { /* 20 Mhz */ {  175.5,    /* SGI */  195.0, },
    /* 40 Mhz */ {  364.5,    /* SGI */  405.0, },
  },

  /* MCS 23  */
  { /* 20 Mhz */ {  195.0,    /* SGI */  216.7, },
    /* 40 Mhz */ {  405.0,    /* SGI */  450.0, },
  },

  /* MCS 24  */
  { /* 20 Mhz */ {   26.0,    /* SGI */   28.9, },
    /* 40 Mhz */ {   54.0,    /* SGI */   60.0, },
  },

  /* MCS 25  */
  { /* 20 Mhz */ {   52.0,    /* SGI */   57.8, },
    /* 40 Mhz */ {  108.0,    /* SGI */  120.0, },
  },

  /* MCS 26  */
  { /* 20 Mhz */ {   78.0,    /* SGI */   86.7, },
    /* 40 Mhz */ {  162.0,    /* SGI */  180.0, },
  },

  /* MCS 27  */
  { /* 20 Mhz */ {  104.0,    /* SGI */  115.6, },
    /* 40 Mhz */ {  216.0,    /* SGI */  240.0, },
  },

  /* MCS 28  */
  { /* 20 Mhz */ {  156.0,    /* SGI */  173.3, },
    /* 40 Mhz */ {  324.0,    /* SGI */  360.0, },
  },

  /* MCS 29  */
  { /* 20 Mhz */ {  208.0,    /* SGI */  231.1, },
    /* 40 Mhz */ {  432.0,    /* SGI */  480.0, },
  },

  /* MCS 30  */
  { /* 20 Mhz */ {  234.0,    /* SGI */  260.0, },
    /* 40 Mhz */ {  486.0,    /* SGI */  540.0, },
  },

  /* MCS 31  */
  { /* 20 Mhz */ {  260.0,    /* SGI */  288.9, },
    /* 40 Mhz */ {  540.0,    /* SGI */  600.0, },
  },

  /* MCS 32  */
  { /* 20 Mhz */ {    0.0,    /* SGI */    0.0, }, /* not valid */
    /* 40 Mhz */ {    6.0,    /* SGI */    6.7, },
  },

  /* MCS 33  */
  { /* 20 Mhz */ {   39.0,    /* SGI */   43.3, },
    /* 40 Mhz */ {   81.0,    /* SGI */   90.0, },
  },

  /* MCS 34  */
  { /* 20 Mhz */ {   52.0,    /* SGI */   57.8, },
    /* 40 Mhz */ {  108.0,    /* SGI */  120.0, },
  },

  /* MCS 35  */
  { /* 20 Mhz */ {   65.0,    /* SGI */   72.2, },
    /* 40 Mhz */ {  135.0,    /* SGI */  150.0, },
  },

  /* MCS 36  */
  { /* 20 Mhz */ {   58.5,    /* SGI */   65.0, },
    /* 40 Mhz */ {  121.5,    /* SGI */  135.0, },
  },

  /* MCS 37  */
  { /* 20 Mhz */ {   78.0,    /* SGI */   86.7, },
    /* 40 Mhz */ {  162.0,    /* SGI */  180.0, },
  },

  /* MCS 38  */
  { /* 20 Mhz */ {   97.5,    /* SGI */  108.3, },
    /* 40 Mhz */ {  202.5,    /* SGI */  225.0, },
  },

  /* MCS 39  */
  { /* 20 Mhz */ {   52.0,    /* SGI */   57.8, },
    /* 40 Mhz */ {  108.0,    /* SGI */  120.0, },
  },

  /* MCS 40  */
  { /* 20 Mhz */ {   65.0,    /* SGI */   72.2, },
    /* 40 Mhz */ {  135.0,    /* SGI */  150.0, },
  },

  /* MCS 41  */
  { /* 20 Mhz */ {   65.0,    /* SGI */   72.2, },
    /* 40 Mhz */ {  135.0,    /* SGI */  150.0, },
  },

  /* MCS 42  */
  { /* 20 Mhz */ {   78.0,    /* SGI */   86.7, },
    /* 40 Mhz */ {  162.0,    /* SGI */  180.0, },
  },

  /* MCS 43  */
  { /* 20 Mhz */ {   91.0,    /* SGI */  101.1, },
    /* 40 Mhz */ {  189.0,    /* SGI */  210.0, },
  },

  /* MCS 44  */
  { /* 20 Mhz */ {   91.0,    /* SGI */  101.1, },
    /* 40 Mhz */ {  189.0,    /* SGI */  210.0, },
  },

  /* MCS 45  */
  { /* 20 Mhz */ {  104.0,    /* SGI */  115.6, },
    /* 40 Mhz */ {  216.0,    /* SGI */  240.0, },
  },

  /* MCS 46  */
  { /* 20 Mhz */ {   78.0,    /* SGI */   86.7, },
    /* 40 Mhz */ {  162.0,    /* SGI */  180.0, },
  },

  /* MCS 47  */
  { /* 20 Mhz */ {   97.5,    /* SGI */  108.3, },
    /* 40 Mhz */ {  202.5,    /* SGI */  225.0, },
  },

  /* MCS 48  */
  { /* 20 Mhz */ {   97.5,    /* SGI */  108.3, },
    /* 40 Mhz */ {  202.5,    /* SGI */  225.0, },
  },

  /* MCS 49  */
  { /* 20 Mhz */ {  117.0,    /* SGI */  130.0, },
    /* 40 Mhz */ {  243.0,    /* SGI */  270.0, },
  },

  /* MCS 50  */
  { /* 20 Mhz */ {  136.5,    /* SGI */  151.7, },
    /* 40 Mhz */ {  283.5,    /* SGI */  315.0, },
  },

  /* MCS 51  */
  { /* 20 Mhz */ {  136.5,    /* SGI */  151.7, },
    /* 40 Mhz */ {  283.5,    /* SGI */  315.0, },
  },

  /* MCS 52  */
  { /* 20 Mhz */ {  156.0,    /* SGI */  173.3, },
    /* 40 Mhz */ {  324.0,    /* SGI */  360.0, },
  },

  /* MCS 53  */
  { /* 20 Mhz */ {   65.0,    /* SGI */   72.2, },
    /* 40 Mhz */ {  135.0,    /* SGI */  150.0, },
  },

  /* MCS 54  */
  { /* 20 Mhz */ {   78.0,    /* SGI */   86.7, },
    /* 40 Mhz */ {  162.0,    /* SGI */  180.0, },
  },

  /* MCS 55  */
  { /* 20 Mhz */ {   91.0,    /* SGI */  101.1, },
    /* 40 Mhz */ {  189.0,    /* SGI */  210.0, },
  },

  /* MCS 56  */
  { /* 20 Mhz */ {   78.0,    /* SGI */   86.7, },
    /* 40 Mhz */ {  162.0,    /* SGI */  180.0, },
  },

  /* MCS 57  */
  { /* 20 Mhz */ {   91.0,    /* SGI */  101.1, },
    /* 40 Mhz */ {  189.0,    /* SGI */  210.0, },
  },

  /* MCS 58  */
  { /* 20 Mhz */ {  104.0,    /* SGI */  115.6, },
    /* 40 Mhz */ {  216.0,    /* SGI */  240.0, },
  },

  /* MCS 59  */
  { /* 20 Mhz */ {  117.0,    /* SGI */  130.0, },
    /* 40 Mhz */ {  243.0,    /* SGI */  270.0, },
  },

  /* MCS 60  */
  { /* 20 Mhz */ {  104.0,    /* SGI */  115.6, },
    /* 40 Mhz */ {  216.0,    /* SGI */  240.0, },
  },

  /* MCS 61  */
  { /* 20 Mhz */ {  117.0,    /* SGI */  130.0, },
    /* 40 Mhz */ {  243.0,    /* SGI */  270.0, },
  },

  /* MCS 62  */
  { /* 20 Mhz */ {  130.0,    /* SGI */  144.4, },
    /* 40 Mhz */ {  270.0,    /* SGI */  300.0, },
  },

  /* MCS 63  */
  { /* 20 Mhz */ {  130.0,    /* SGI */  144.4, },
    /* 40 Mhz */ {  270.0,    /* SGI */  300.0, },
  },

  /* MCS 64  */
  { /* 20 Mhz */ {  143.0,    /* SGI */  158.9, },
    /* 40 Mhz */ {  297.0,    /* SGI */  330.0, },
  },

  /* MCS 65  */
  { /* 20 Mhz */ {   97.5,    /* SGI */  108.3, },
    /* 40 Mhz */ {  202.5,    /* SGI */  225.0, },
  },

  /* MCS 66  */
  { /* 20 Mhz */ {  117.0,    /* SGI */  130.0, },
    /* 40 Mhz */ {  243.0,    /* SGI */  270.0, },
  },

  /* MCS 67  */
  { /* 20 Mhz */ {  136.5,    /* SGI */  151.7, },
    /* 40 Mhz */ {  283.5,    /* SGI */  315.0, },
  },

  /* MCS 68  */
  { /* 20 Mhz */ {  117.0,    /* SGI */  130.0, },
    /* 40 Mhz */ {  243.0,    /* SGI */  270.0, },
  },

  /* MCS 69  */
  { /* 20 Mhz */ {  136.5,    /* SGI */  151.7, },
    /* 40 Mhz */ {  283.5,    /* SGI */  315.0, },
  },

  /* MCS 70  */
  { /* 20 Mhz */ {  156.0,    /* SGI */  173.3, },
    /* 40 Mhz */ {  324.0,    /* SGI */  360.0, },
  },

  /* MCS 71  */
  { /* 20 Mhz */ {  175.5,    /* SGI */  195.0, },
    /* 40 Mhz */ {  364.5,    /* SGI */  405.0, },
  },

  /* MCS 72  */
  { /* 20 Mhz */ {  156.0,    /* SGI */  173.3, },
    /* 40 Mhz */ {  324.0,    /* SGI */  360.0, },
  },

  /* MCS 73  */
  { /* 20 Mhz */ {  175.5,    /* SGI */  195.0, },
    /* 40 Mhz */ {  364.5,    /* SGI */  405.0, },
  },

  /* MCS 74  */
  { /* 20 Mhz */ {  195.0,    /* SGI */  216.7, },
    /* 40 Mhz */ {  405.0,    /* SGI */  450.0, },
  },

  /* MCS 75  */
  { /* 20 Mhz */ {  195.0,    /* SGI */  216.7, },
    /* 40 Mhz */ {  405.0,    /* SGI */  450.0, },
  },

  /* MCS 76  */
  { /* 20 Mhz */ {  214.5,    /* SGI */  238.3, },
    /* 40 Mhz */ {  445.5,    /* SGI */  495.0, },
  },
};

#define CHAN_FHSS (CHAN_2GHZ | CHAN_GFSK)
#define CHAN_A (CHAN_5GHZ | CHAN_OFDM)
#define CHAN_B (CHAN_2GHZ | CHAN_CCK)
#define CHAN_PUREG (CHAN_2GHZ | CHAN_OFDM)
#define CHAN_G (CHAN_2GHZ | CHAN_DYN)

#define IS_CHAN_FHSS(flags) ((flags & CHAN_FHSS) == CHAN_FHSS)
#define IS_CHAN_A(flags) ((flags & CHAN_A) == CHAN_A)
#define IS_CHAN_B(flags) ((flags & CHAN_B) == CHAN_B)
#define IS_CHAN_PUREG(flags) ((flags & CHAN_PUREG) == CHAN_PUREG)
#define IS_CHAN_G(flags) ((flags & CHAN_G) == CHAN_G)
#define IS_CHAN_ANYG(flags) (IS_CHAN_PUREG(flags) || IS_CHAN_G(flags))

static void add_chaninfo(Local<Object> obj, int freq, int flags) {

  // TODO: use array for flags

  SET_INT(obj, "channelFreqMHz", freq);

  std::string s = "";
  if (IS_CHAN_FHSS(flags))
    s += "FHSS";
  if (IS_CHAN_A(flags)) {
    if (flags & CHAN_HALF)
      s +=  " 11a/10Mhz";
    else if (flags & CHAN_QUARTER)
      s +=  " 11a/5Mhz";
    else
      s +=  " 11a";
  }
  if (IS_CHAN_ANYG(flags)) {
    if (flags & CHAN_HALF)
      s += "11g/10Mhz";
    else if (flags & CHAN_QUARTER)
      s += "11g/5Mhz";
    else
      s += "11g";
  } else if (IS_CHAN_B(flags))
    s += "11b";
  if (flags & CHAN_TURBO)
    s += "Turbo";
  if (flags & CHAN_HT20)
    s += "ht/20";
  else if (flags & CHAN_HT40D)
    s += "ht/40-";
  else if (flags & CHAN_HT40U)
    s += "ht/40+";

  SET_STR(obj, "channelFlags", s);

}

static int add_field(
  Local<Object> obj,
  struct cpack_state *s,
  uint32_t bit,
  uint8_t *flags,
  struct radiotap_state *state,
  uint32_t presentflags
) {

  union {
    int8_t    i8;
    uint8_t   u8;
    int16_t   i16;
    uint16_t  u16;
    uint32_t  u32;
    uint64_t  u64;
  } u, u2, u3, u4;
  int rc;

  switch (bit) {
  case RTAP_FLAGS:
    rc = cpack_uint8(s, &u.u8);
    if (rc != 0)
      break;
    *flags = u.u8;
    break;
  case RTAP_RATE:
    rc = cpack_uint8(s, &u.u8);
    if (rc != 0)
      break;

    /* Save state rate */
    state->rate = u.u8;
    break;
  case RTAP_DB_ANTSIGNAL:
  case RTAP_DB_ANTNOISE:
  case RTAP_ANTENNA:
    rc = cpack_uint8(s, &u.u8);
    break;
  case RTAP_DBM_ANTSIGNAL:
  case RTAP_DBM_ANTNOISE:
    rc = cpack_int8(s, &u.i8);
    break;
  case RTAP_CHANNEL:
    rc = cpack_uint16(s, &u.u16);
    if (rc != 0)
      break;
    rc = cpack_uint16(s, &u2.u16);
    break;
  case RTAP_FHSS:
  case RTAP_LOCK_QUALITY:
  case RTAP_TX_ATTENUATION:
  case RTAP_RX_FLAGS:
    rc = cpack_uint16(s, &u.u16);
    break;
  case RTAP_DB_TX_ATTENUATION:
    rc = cpack_uint8(s, &u.u8);
    break;
  case RTAP_DBM_TX_POWER:
    rc = cpack_int8(s, &u.i8);
    break;
  case RTAP_TSFT:
    rc = cpack_uint64(s, &u.u64);
    break;
  case RTAP_XCHANNEL:
    rc = cpack_uint32(s, &u.u32);
    if (rc != 0)
      break;
    rc = cpack_uint16(s, &u2.u16);
    if (rc != 0)
      break;
    rc = cpack_uint8(s, &u3.u8);
    if (rc != 0)
      break;
    rc = cpack_uint8(s, &u4.u8);
    break;
  case RTAP_MCS:
    rc = cpack_uint8(s, &u.u8);
    if (rc != 0)
      break;
    rc = cpack_uint8(s, &u2.u8);
    if (rc != 0)
      break;
    rc = cpack_uint8(s, &u3.u8);
    break;
  case RTAP_VENDOR_NAMESPACE: {
    uint8_t vns[3];
    uint16_t length;
    uint8_t subspace;

    if ((cpack_align_and_reserve(s, 2)) == NULL) {
      rc = -1;
      break;
    }

    rc = cpack_uint8(s, &vns[0]);
    if (rc != 0)
      break;
    rc = cpack_uint8(s, &vns[1]);
    if (rc != 0)
      break;
    rc = cpack_uint8(s, &vns[2]);
    if (rc != 0)
      break;
    rc = cpack_uint8(s, &subspace);
    if (rc != 0)
      break;
    rc = cpack_uint16(s, &length);
    if (rc != 0)
      break;

    /* Skip up to length */
    s->c_next += length;
    break;
  }
  default:
    /* this bit indicates a field whose
     * size we do not know, so we cannot
     * proceed.  Just print the bit number.
     */
    return -1;
  }

  if (rc != 0) {
    return rc;
  }

  /* Preserve the state present flags */
  state->present = presentflags;

  switch (bit) {
  case RTAP_CHANNEL:
    /*
     * If CHANNEL and XCHANNEL are both present, skip
     * CHANNEL.
     */
    if (presentflags & (1 << RTAP_XCHANNEL))
      break;
    add_chaninfo(obj, u.u16, u2.u16);
    break;
  case RTAP_FHSS:
    SET_INT(obj, "fhset", u.u16 & 0xff);
    SET_INT(obj, "fhpat", (u.u16 >> 8) & 0xff);
    break;
  case RTAP_RATE:
    if (u.u8 >= 0x80 && u.u8 <= 0x8f) {
      SET_INT(obj, "mcs", u.u8 & 0x7f);
    } else
      // ND_PRINT((ndo, "%2.1f Mb/s ", .5 * u.u8)); TODO
    break;
  case RTAP_DBM_ANTSIGNAL:
    SET_INT(obj, "signalDb", u.i8);
    break;
  case RTAP_DBM_ANTNOISE:
    SET_INT(obj, "noiseDb", u.i8);
    break;
  case RTAP_DB_ANTSIGNAL:
    SET_INT(obj, "signalDb", u.u8);
    break;
  case RTAP_DB_ANTNOISE:
    SET_INT(obj, "noiseDb", u.u8);
    break;
  case RTAP_LOCK_QUALITY:
    SET_INT(obj, "sq", u.u16);
    break;
  case RTAP_TX_ATTENUATION:
    SET_INT(obj, "txPower", -(int) u.u16);
    break;
  case RTAP_DB_TX_ATTENUATION:
    SET_INT(obj, "txPowerDb", -(int) u.u8);
    break;
  case RTAP_DBM_TX_POWER:
    SET_INT(obj, "txPowerDbm", u.i8);
    break;
  case RTAP_FLAGS:
    SET_BOOL(obj, "cfp", u.u8 & RTAP_F_CFP);
    SET_BOOL(obj, "shortPreamble", u.u8 & RTAP_F_SHORTPRE);
    SET_BOOL(obj, "wep", u.u8 & RTAP_F_WEP);
    SET_BOOL(obj, "fragmented", u.u8 & RTAP_F_FRAG);
    SET_BOOL(obj, "badFcs", u.u8 & RTAP_F_BADFCS);
    break;
  case RTAP_ANTENNA:
    SET_INT(obj, "antenna", u.u8);
    break;
  case RTAP_TSFT:
    // ND_PRINT((ndo, "%" PRIu64 "us tsft ", u.u64)); TODO
    break;
  case RTAP_RX_FLAGS:
    /* Do nothing for now */
    break;
  case RTAP_XCHANNEL:
    add_chaninfo(obj, u2.u16, u.u32);
    break;
  /*
  case RTAP_MCS: { // TODO;
    static const char *bandwidth[4] = {
      "20 MHz",
      "40 MHz",
      "20 MHz (L)",
      "20 MHz (U)"
    };
    float htrate;

    if (u.u8 & RTAP_MCS_MCS_INDEX_KNOWN) {
      if (u3.u8 <= MAX_MCS_INDEX) {
        if (u.u8 & (RTAP_MCS_BANDWIDTH_KNOWN|RTAP_MCS_GUARD_INTERVAL_KNOWN)) {
          htrate =
            ieee80211_float_htrates \
              [u3.u8] \
              [((u2.u8 & RTAP_MCS_BANDWIDTH_MASK) == RTAP_MCS_BANDWIDTH_40 ? 1 : 0)] \
              [((u2.u8 & RTAP_MCS_SHORT_GI) ? 1 : 0)];
        } else {
          htrate = 0.0;
        }
      } else {
        htrate = 0.0;
      }
      if (htrate != 0.0) {
        ND_PRINT((ndo, "%.1f Mb/s MCS %u ", htrate, u3.u8));
      } else {
        ND_PRINT((ndo, "MCS %u ", u3.u8));
      }
    }
    if (u.u8 & RTAP_MCS_BANDWIDTH_KNOWN) {
      ND_PRINT((ndo, "%s ",
        bandwidth[u2.u8 & RTAP_MCS_BANDWIDTH_MASK]));
    }
    if (u.u8 & RTAP_MCS_GUARD_INTERVAL_KNOWN) {
      ND_PRINT((ndo, "%s GI ",
        (u2.u8 & RTAP_MCS_SHORT_GI) ?
        "short" : "lon"));
    }
    if (u.u8 & RTAP_MCS_HT_FORMAT_KNOWN) {
      ND_PRINT((ndo, "%s ",
        (u2.u8 & RTAP_MCS_HT_GREENFIELD) ?
        "greenfield" : "mixed"));
    }
    if (u.u8 & RTAP_MCS_FEC_TYPE_KNOWN) {
      ND_PRINT((ndo, "%s FEC ",
        (u2.u8 & RTAP_MCS_FEC_LDPC) ?
        "LDPC" : "BCC"));
    }
    if (u.u8 & RTAP_MCS_STBC_KNOWN) {
      ND_PRINT((ndo, "RX-STBC%u ",
        (u2.u8 & RTAP_MCS_STBC_MASK) >> RTAP_MCS_STBC_SHIFT));
    }

    break;
    } */
  }
  return 0;

}

NAN_METHOD(decode_radiotap) {

  NanScope();
  precondition(
    args.Length() == 2 &&
    node::Buffer::HasInstance(args[0]) &&
    args[1]->IsInt32()
  );

  Local<Object> frame = NanNew<Object>();
  Local<Object> packet = args[0]->ToObject();
  u_char *p = (u_char *) node::Buffer::Data(packet);
  // u_int length = node::Buffer::Length(packet);

  // SET_BOOL(frame, "b", false);
  // SET_INT(frame, "i", 123);
  // SET_STR(frame, "s", "hello");
  // SET_NULL(frame, "n");

#define BITNO_32(x) (((x) >> 16) ? 16 + BITNO_16((x) >> 16) : BITNO_16((x)))
#define BITNO_16(x) (((x) >> 8) ? 8 + BITNO_8((x) >> 8) : BITNO_8((x)))
#define BITNO_8(x) (((x) >> 4) ? 4 + BITNO_4((x) >> 4) : BITNO_4((x)))
#define BITNO_4(x) (((x) >> 2) ? 2 + BITNO_2((x) >> 2) : BITNO_2((x)))
#define BITNO_2(x) (((x) & 2) ? 1 : 0)
#define BIT(n)  (1U << n)
#define IS_EXTENDED(__p) (EXTRACT_LE_32BITS(__p) & BIT(RTAP_EXT)) != 0

  struct cpack_state cpacker;
  struct ieee80211_radiotap_header *hdr;
  uint32_t present, next_present;
  uint32_t presentflags = 0;
  uint32_t *presentp, *last_presentp;
  enum ieee80211_radiotap_type bit;
  int bit0;
  u_int len;
  uint8_t flags;
  int pad;
  u_int fcslen;
  struct radiotap_state state;

  hdr = (struct ieee80211_radiotap_header *) p;
  len = EXTRACT_LE_16BITS(&hdr->it_len);

  cpack_init(&cpacker, (uint8_t *)hdr, len); /* align against header start */
  cpack_advance(&cpacker, sizeof(*hdr)); /* includes the 1st bitmap */
  for (last_presentp = &hdr->it_present;
       IS_EXTENDED(last_presentp) &&
       (u_char*)(last_presentp + 1) <= p + len;
       last_presentp++)
    cpack_advance(&cpacker, sizeof(hdr->it_present)); /* more bitmaps */

  /* are there more bitmap extensions than bytes in header? */
  if (IS_EXTENDED(last_presentp)) {
    return NanNull();
  }

  /* Assume no flags */
  flags = 0;
  /* Assume no Atheros padding between 802.11 header and body */
  pad = 0;
  /* Assume no FCS at end of frame */
  fcslen = 0;
  for (bit0 = 0, presentp = &hdr->it_present; presentp <= last_presentp;
       presentp++, bit0 += 32) {
    presentflags = EXTRACT_LE_32BITS(presentp);

    /* Clear state. */
    memset(&state, 0, sizeof(state));

    for (present = EXTRACT_LE_32BITS(presentp); present;
         present = next_present) {
      /* clear the least significant bit that is set */
      next_present = present & (present - 1);

      /* extract the least significant bit that is set */
      bit = (enum ieee80211_radiotap_type)
          (bit0 + BITNO_32(present ^ next_present));

      if (add_field(frame, &cpacker, bit, &flags, &state, presentflags) != 0)
        break;
    }
  }

  if (flags & RTAP_F_DATAPAD)
    pad = 1;  /* Atheros padding */
  if (flags & RTAP_F_FCS)
    fcslen = 4; /* FCS at end of packet */

  // TODO: Set body as slice of original buffer.
  // return len + ieee802_11_print(ndo, p + len, length - len, caplen - len, pad, fcslen);

#undef BITNO_32
#undef BITNO_16
#undef BITNO_8
#undef BITNO_4
#undef BITNO_2
#undef BIT

  NanReturnValue(frame);

}
