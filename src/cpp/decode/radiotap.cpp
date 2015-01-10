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

#include <string.h>

enum ieee80211_radiotap_type {
  IEEE80211_RADIOTAP_TSFT = 0,
  IEEE80211_RADIOTAP_FLAGS = 1,
  IEEE80211_RADIOTAP_RATE = 2,
  IEEE80211_RADIOTAP_CHANNEL = 3,
  IEEE80211_RADIOTAP_FHSS = 4,
  IEEE80211_RADIOTAP_DBM_ANTSIGNAL = 5,
  IEEE80211_RADIOTAP_DBM_ANTNOISE = 6,
  IEEE80211_RADIOTAP_LOCK_QUALITY = 7,
  IEEE80211_RADIOTAP_TX_ATTENUATION = 8,
  IEEE80211_RADIOTAP_DB_TX_ATTENUATION = 9,
  IEEE80211_RADIOTAP_DBM_TX_POWER = 10,
  IEEE80211_RADIOTAP_ANTENNA = 11,
  IEEE80211_RADIOTAP_DB_ANTSIGNAL = 12,
  IEEE80211_RADIOTAP_DB_ANTNOISE = 13,
  IEEE80211_RADIOTAP_RX_FLAGS = 14,
  /* NB: gap for netbsd definitions */
  IEEE80211_RADIOTAP_XCHANNEL = 18,
  IEEE80211_RADIOTAP_MCS = 19,
  IEEE80211_RADIOTAP_NAMESPACE = 29,
  IEEE80211_RADIOTAP_VENDOR_NAMESPACE = 30,
  IEEE80211_RADIOTAP_EXT = 31
};

/* channel attributes */
#define IEEE80211_CHAN_TURBO  0x00010 /* Turbo channel */
#define IEEE80211_CHAN_CCK  0x00020 /* CCK channel */
#define IEEE80211_CHAN_OFDM 0x00040 /* OFDM channel */
#define IEEE80211_CHAN_2GHZ 0x00080 /* 2 GHz spectrum channel. */
#define IEEE80211_CHAN_5GHZ 0x00100 /* 5 GHz spectrum channel */
#define IEEE80211_CHAN_PASSIVE  0x00200 /* Only passive scan allowed */
#define IEEE80211_CHAN_DYN  0x00400 /* Dynamic CCK-OFDM channel */
#define IEEE80211_CHAN_GFSK 0x00800 /* GFSK channel (FHSS PHY) */
#define IEEE80211_CHAN_GSM  0x01000 /* 900 MHz spectrum channel */
#define IEEE80211_CHAN_STURBO 0x02000 /* 11a static turbo channel only */
#define IEEE80211_CHAN_HALF 0x04000 /* Half rate channel */
#define IEEE80211_CHAN_QUARTER  0x08000 /* Quarter rate channel */
#define IEEE80211_CHAN_HT20 0x10000 /* HT 20 channel */
#define IEEE80211_CHAN_HT40U  0x20000 /* HT 40 channel w/ ext above */
#define IEEE80211_CHAN_HT40D  0x40000 /* HT 40 channel w/ ext below */

/* Useful combinations of channel characteristics, borrowed from Ethereal */
#define IEEE80211_CHAN_A \
        (IEEE80211_CHAN_5GHZ | IEEE80211_CHAN_OFDM)
#define IEEE80211_CHAN_B \
        (IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_CCK)
#define IEEE80211_CHAN_G \
        (IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_DYN)
#define IEEE80211_CHAN_TA \
        (IEEE80211_CHAN_5GHZ | IEEE80211_CHAN_OFDM | IEEE80211_CHAN_TURBO)
#define IEEE80211_CHAN_TG \
        (IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_DYN  | IEEE80211_CHAN_TURBO)


/* For IEEE80211_RADIOTAP_FLAGS */
#define IEEE80211_RADIOTAP_F_CFP  0x01  /* sent/received
             * during CFP
             */
#define IEEE80211_RADIOTAP_F_SHORTPRE 0x02  /* sent/received
             * with short
             * preamble
             */
#define IEEE80211_RADIOTAP_F_WEP  0x04  /* sent/received
             * with WEP encryption
             */
#define IEEE80211_RADIOTAP_F_FRAG 0x08  /* sent/received
             * with fragmentation
             */
#define IEEE80211_RADIOTAP_F_FCS  0x10  /* frame includes FCS */
#define IEEE80211_RADIOTAP_F_DATAPAD  0x20  /* frame has padding between
             * 802.11 header and payload
             * (to 32-bit boundary)
             */
#define IEEE80211_RADIOTAP_F_BADFCS 0x40  /* does not pass FCS check */

/* For IEEE80211_RADIOTAP_RX_FLAGS */
#define IEEE80211_RADIOTAP_F_RX_BADFCS  0x0001  /* frame failed crc check */
#define IEEE80211_RADIOTAP_F_RX_PLCP_CRC  0x0002  /* frame failed PLCP CRC check */

/* For IEEE80211_RADIOTAP_MCS known */
#define IEEE80211_RADIOTAP_MCS_BANDWIDTH_KNOWN    0x01
#define IEEE80211_RADIOTAP_MCS_MCS_INDEX_KNOWN    0x02  /* MCS index field */
#define IEEE80211_RADIOTAP_MCS_GUARD_INTERVAL_KNOWN 0x04
#define IEEE80211_RADIOTAP_MCS_HT_FORMAT_KNOWN    0x08
#define IEEE80211_RADIOTAP_MCS_FEC_TYPE_KNOWN   0x10
#define IEEE80211_RADIOTAP_MCS_STBC_KNOWN   0x20

/* For IEEE80211_RADIOTAP_MCS flags */
#define IEEE80211_RADIOTAP_MCS_BANDWIDTH_MASK 0x03
#define IEEE80211_RADIOTAP_MCS_BANDWIDTH_20 0
#define IEEE80211_RADIOTAP_MCS_BANDWIDTH_40 1
#define IEEE80211_RADIOTAP_MCS_BANDWIDTH_20L  2
#define IEEE80211_RADIOTAP_MCS_BANDWIDTH_20U  3
#define IEEE80211_RADIOTAP_MCS_SHORT_GI   0x04 /* short guard interval */
#define IEEE80211_RADIOTAP_MCS_HT_GREENFIELD  0x08
#define IEEE80211_RADIOTAP_MCS_FEC_LDPC   0x10
#define IEEE80211_RADIOTAP_MCS_STBC_MASK  0x60
#define   IEEE80211_RADIOTAP_MCS_STBC_1 1
#define   IEEE80211_RADIOTAP_MCS_STBC_2 2
#define   IEEE80211_RADIOTAP_MCS_STBC_3 3
#define IEEE80211_RADIOTAP_MCS_STBC_SHIFT 5

#define PRINT_SSID(p) \
  if (p.ssid_present) { \
    ND_PRINT((ndo, " (")); \
    fn_print(ndo, p.ssid.ssid, NULL); \
    ND_PRINT((ndo, ")")); \
  }

#define PRINT_RATE(_sep, _r, _suf) \
  ND_PRINT((ndo, "%s%2.1f%s", _sep, (.5 * ((_r) & 0x7f)), _suf))
#define PRINT_RATES(p) \
  if (p.rates_present) { \
    int z; \
    const char *sep = " ["; \
    for (z = 0; z < p.rates.length ; z++) { \
      PRINT_RATE(sep, p.rates.rate[z], \
        (p.rates.rate[z] & 0x80 ? "*" : "")); \
      sep = " "; \
    } \
    if (p.rates.length != 0) \
      ND_PRINT((ndo, " Mbit]")); \
  }

#define PRINT_DS_CHANNEL(p) \
  if (p.ds_present) \
    ND_PRINT((ndo, " CH: %u", p.ds.channel)); \
  ND_PRINT((ndo, "%s", \
      CAPABILITY_PRIVACY(p.capability_info) ? ", PRIVACY" : ""));

#define MAX_MCS_INDEX 76

/*
 * Indices are:
 *
 *  the MCS index (0-76);
 *
 *  0 for 20 MHz, 1 for 40 MHz;
 *
 *  0 for a long guard interval, 1 for a short guard interval.
 */
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

static const char *auth_alg_text[]={"Open System","Shared Key","EAP"};
#define NUM_AUTH_ALGS (sizeof auth_alg_text / sizeof auth_alg_text[0])

static const char *status_text[] = {
  "Successful",           /*  0 */
  "Unspecified failure",          /*  1 */
  "Reserved",           /*  2 */
  "Reserved",           /*  3 */
  "Reserved",           /*  4 */
  "Reserved",           /*  5 */
  "Reserved",           /*  6 */
  "Reserved",           /*  7 */
  "Reserved",           /*  8 */
  "Reserved",           /*  9 */
  "Cannot Support all requested capabilities in the Capability "
    "Information field",            /* 10 */
  "Reassociation denied due to inability to confirm that association "
    "exists",           /* 11 */
  "Association denied due to reason outside the scope of the "
    "standard",           /* 12 */
  "Responding station does not support the specified authentication "
    "algorithm ",           /* 13 */
  "Received an Authentication frame with authentication transaction "
    "sequence number out of expected sequence",   /* 14 */
  "Authentication rejected because of challenge failure", /* 15 */
  "Authentication rejected due to timeout waiting for next frame in "
    "sequence",             /* 16 */
  "Association denied because AP is unable to handle additional"
    "associated stations",          /* 17 */
  "Association denied due to requesting station not supporting all of "
    "the data rates in BSSBasicRateSet parameter",  /* 18 */
  "Association denied due to requesting station not supporting "
    "short preamble operation",       /* 19 */
  "Association denied due to requesting station not supporting "
    "PBCC encoding",          /* 20 */
  "Association denied due to requesting station not supporting "
    "channel agility",          /* 21 */
  "Association request rejected because Spectrum Management "
    "capability is required",       /* 22 */
  "Association request rejected because the information in the "
    "Power Capability element is unacceptable",   /* 23 */
  "Association request rejected because the information in the "
    "Supported Channels element is unacceptable",   /* 24 */
  "Association denied due to requesting station not supporting "
    "short slot operation",       /* 25 */
  "Association denied due to requesting station not supporting "
    "DSSS-OFDM operation",        /* 26 */
  "Association denied because the requested STA does not support HT "
    "features",           /* 27 */
  "Reserved",           /* 28 */
  "Association denied because the requested STA does not support "
    "the PCO transition time required by the AP",   /* 29 */
  "Reserved",           /* 30 */
  "Reserved",           /* 31 */
  "Unspecified, QoS-related failure",     /* 32 */
  "Association denied due to QAP having insufficient bandwidth "
    "to handle another QSTA",       /* 33 */
  "Association denied due to excessive frame loss rates and/or "
    "poor conditions on current operating channel", /* 34 */
  "Association (with QBSS) denied due to requesting station not "
    "supporting the QoS facility",      /* 35 */
  "Association denied due to requesting station not supporting "
    "Block Ack",            /* 36 */
  "The request has been declined",      /* 37 */
  "The request has not been successful as one or more parameters "
    "have invalid values",        /* 38 */
  "The TS has not been created because the request cannot be honored. "
    "Try again with the suggested changes to the TSPEC",  /* 39 */
  "Invalid Information Element",        /* 40 */
  "Group Cipher is not valid",        /* 41 */
  "Pairwise Cipher is not valid",       /* 42 */
  "AKMP is not valid",          /* 43 */
  "Unsupported RSN IE version",       /* 44 */
  "Invalid RSN IE Capabilities",        /* 45 */
  "Cipher suite is rejected per security policy",   /* 46 */
  "The TS has not been created. However, the HC may be capable of "
    "creating a TS, in response to a request, after the time indicated "
    "in the TS Delay element",        /* 47 */
  "Direct Link is not allowed in the BSS by policy",  /* 48 */
  "Destination STA is not present within this QBSS.", /* 49 */
  "The Destination STA is not a QSTA.",     /* 50 */

};
#define NUM_STATUSES  (sizeof status_text / sizeof status_text[0])

static const char *reason_text[] = {
  "Reserved",           /* 0 */
  "Unspecified reason",         /* 1 */
  "Previous authentication no longer valid",      /* 2 */
  "Deauthenticated because sending station is leaving (or has left) "
    "IBSS or ESS",          /* 3 */
  "Disassociated due to inactivity",      /* 4 */
  "Disassociated because AP is unable to handle all currently "
    " associated stations",       /* 5 */
  "Class 2 frame received from nonauthenticated station", /* 6 */
  "Class 3 frame received from nonassociated station",  /* 7 */
  "Disassociated because sending station is leaving "
    "(or has left) BSS",          /* 8 */
  "Station requesting (re)association is not authenticated with "
    "responding station",         /* 9 */
  "Disassociated because the information in the Power Capability "
    "element is unacceptable",        /* 10 */
  "Disassociated because the information in the SupportedChannels "
    "element is unacceptable",        /* 11 */
  "Invalid Information Element",        /* 12 */
  "Reserved",           /* 13 */
  "Michael MIC failure",          /* 14 */
  "4-Way Handshake timeout",        /* 15 */
  "Group key update timeout",       /* 16 */
  "Information element in 4-Way Handshake different from (Re)Association"
    "Request/Probe Response/Beacon",      /* 17 */
  "Group Cipher is not valid",        /* 18 */
  "AKMP is not valid",          /* 20 */
  "Unsupported RSN IE version",       /* 21 */
  "Invalid RSN IE Capabilities",        /* 22 */
  "IEEE 802.1X Authentication failed",      /* 23 */
  "Cipher suite is rejected per security policy",   /* 24 */
  "Reserved",           /* 25 */
  "Reserved",           /* 26 */
  "Reserved",           /* 27 */
  "Reserved",           /* 28 */
  "Reserved",           /* 29 */
  "Reserved",           /* 30 */
  "TS deleted because QoS AP lacks sufficient bandwidth for this "
    "QoS STA due to a change in BSS service characteristics or "
    "operational mode (e.g. an HT BSS change from 40 MHz channel "
    "to 20 MHz channel)",         /* 31 */
  "Disassociated for unspecified, QoS-related reason",  /* 32 */
  "Disassociated because QoS AP lacks sufficient bandwidth for this "
    "QoS STA",            /* 33 */
  "Disassociated because of excessive number of frames that need to be "
          "acknowledged, but are not acknowledged for AP transmissions "
    "and/or poor channel conditions",     /* 34 */
  "Disassociated because STA is transmitting outside the limits "
    "of its TXOPs",         /* 35 */
  "Requested from peer STA as the STA is leaving the BSS "
    "(or resetting)",         /* 36 */
  "Requested from peer STA as it does not want to use the "
    "mechanism",            /* 37 */
  "Requested from peer STA as the STA received frames using the "
    "mechanism for which a set up is required",   /* 38 */
  "Requested from peer STA due to time out",    /* 39 */
  "Reserved",           /* 40 */
  "Reserved",           /* 41 */
  "Reserved",           /* 42 */
  "Reserved",           /* 43 */
  "Reserved",           /* 44 */
  "Peer STA does not support the requested cipher suite", /* 45 */
  "Association denied due to requesting STA not supporting HT "
    "features",           /* 46 */
};
#define NUM_REASONS (sizeof reason_text / sizeof reason_text[0])

#define IEEE80211_CHAN_FHSS \
  (IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_GFSK)
#define IEEE80211_CHAN_A \
  (IEEE80211_CHAN_5GHZ | IEEE80211_CHAN_OFDM)
#define IEEE80211_CHAN_B \
  (IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_CCK)
#define IEEE80211_CHAN_PUREG \
  (IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_OFDM)
#define IEEE80211_CHAN_G \
  (IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_DYN)

#define IS_CHAN_FHSS(flags) \
  ((flags & IEEE80211_CHAN_FHSS) == IEEE80211_CHAN_FHSS)
#define IS_CHAN_A(flags) \
  ((flags & IEEE80211_CHAN_A) == IEEE80211_CHAN_A)
#define IS_CHAN_B(flags) \
  ((flags & IEEE80211_CHAN_B) == IEEE80211_CHAN_B)
#define IS_CHAN_PUREG(flags) \
  ((flags & IEEE80211_CHAN_PUREG) == IEEE80211_CHAN_PUREG)
#define IS_CHAN_G(flags) \
  ((flags & IEEE80211_CHAN_G) == IEEE80211_CHAN_G)
#define IS_CHAN_ANYG(flags) \
  (IS_CHAN_PUREG(flags) || IS_CHAN_G(flags))


#include "macros.hpp"
#include "radiotap.hpp"

using namespace v8;

NAN_METHOD(decode_radiotap) {

  NanScope();
  precondition(
    args.Length() == 2 &&
    node::Buffer::HasInstance(args[0]) &&
    args[1]->IsInt32()
  );

  Local<Object> frame = NanNew<Object>();

  SET_BOOL(frame, "b", false);
  SET_INT(frame, "i", 123);
  SET_STR(frame, "s", "hello");
  SET_NULL(frame, "n");

  NanReturnValue(frame);

}
