/*
 * Copyright (c) 1992, 1993, 1994, 1995, 1996
 *  The Regents of the University of California.  All rights reserved.
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
#ifndef LAYER2_DECODE_MACROS_HPP
#define LAYER2_DECODE_MACROS_HPP

#include <node.h>

#define precondition(b) if (!(b)) return NanThrowError("Illegal arguments.")

#define SET_BOOL(obj, s, v) (obj)->Set(NanNew<String>((s)), NanNew<Boolean>((v)))
#define SET_INT(obj, s, v) (obj)->Set(NanNew<String>((s)), NanNew<Integer>((v)))
#define SET_STR(obj, s, v) (obj)->Set(NanNew<String>((s)), NanNew<String>((v)))
#define SET_NULL(obj, s) (obj)->Set(NanNew<String>((s)), NanNull())

/*
 * Macros to extract possibly-unaligned big-endian integral values.
 */
#ifdef LBL_ALIGN

#if defined(__GNUC__) && defined(HAVE___ATTRIBUTE__) && \
    (defined(__alpha) || defined(__alpha__) || \
     defined(__mips) || defined(__mips__))

typedef struct {
  uint16_t  val;
} __attribute__((packed)) unaligned_uint16_t;

typedef struct {
  uint32_t  val;
} __attribute__((packed)) unaligned_uint32_t;

static inline uint16_t EXTRACT_16BITS(const void *p) {
  return ((uint16_t)ntohs(((const unaligned_uint16_t *)(p))->val));
}

static inline uint32_t EXTRACT_32BITS(const void *p) {
  return ((uint32_t)ntohl(((const unaligned_uint32_t *)(p))->val));
}

static inline uint64_t EXTRACT_64BITS(const void *p) {
  return ((uint64_t)(((uint64_t)ntohl(((const unaligned_uint32_t *)(p) + 0)->val)) << 32 | \
    ((uint64_t)ntohl(((const unaligned_uint32_t *)(p) + 1)->val)) << 0));
}

#else /* have to do it a byte at a time */

#define EXTRACT_16BITS(p) \
  ((uint16_t)(((uint16_t)(*((const uint8_t *)(p) + 0)) << 8) | \
              ((uint16_t)(*((const uint8_t *)(p) + 1)) << 0)))
#define EXTRACT_32BITS(p) \
  ((uint32_t)(((uint32_t)(*((const uint8_t *)(p) + 0)) << 24) | \
              ((uint32_t)(*((const uint8_t *)(p) + 1)) << 16) | \
              ((uint32_t)(*((const uint8_t *)(p) + 2)) << 8) | \
              ((uint32_t)(*((const uint8_t *)(p) + 3)) << 0)))
#define EXTRACT_64BITS(p) \
  ((uint64_t)(((uint64_t)(*((const uint8_t *)(p) + 0)) << 56) | \
              ((uint64_t)(*((const uint8_t *)(p) + 1)) << 48) | \
              ((uint64_t)(*((const uint8_t *)(p) + 2)) << 40) | \
              ((uint64_t)(*((const uint8_t *)(p) + 3)) << 32) | \
              ((uint64_t)(*((const uint8_t *)(p) + 4)) << 24) | \
              ((uint64_t)(*((const uint8_t *)(p) + 5)) << 16) | \
              ((uint64_t)(*((const uint8_t *)(p) + 6)) << 8) | \
              ((uint64_t)(*((const uint8_t *)(p) + 7)) << 0)))

#endif /* must special-case unaligned accesses */

#else /* LBL_ALIGN */

static inline uint16_t EXTRACT_16BITS(const void *p) {
  return ((uint16_t)ntohs(*(const uint16_t *)(p)));
}

static inline uint32_t EXTRACT_32BITS(const void *p) {
  return ((uint32_t)ntohl(*(const uint32_t *)(p)));
}

static inline uint64_t EXTRACT_64BITS(const void *p) {
  return ((uint64_t)(((uint64_t)ntohl(*((const uint32_t *)(p) + 0))) << 32 | \
    ((uint64_t)ntohl(*((const uint32_t *)(p) + 1))) << 0));
}

#endif /* LBL_ALIGN */

#define EXTRACT_24BITS(p) \
  ((uint32_t)(((uint32_t)(*((const uint8_t *)(p) + 0)) << 16) | \
              ((uint32_t)(*((const uint8_t *)(p) + 1)) << 8) | \
              ((uint32_t)(*((const uint8_t *)(p) + 2)) << 0)))

#define EXTRACT_40BITS(p) \
  ((uint64_t)(((uint64_t)(*((const uint8_t *)(p) + 0)) << 32) | \
              ((uint64_t)(*((const uint8_t *)(p) + 1)) << 24) | \
              ((uint64_t)(*((const uint8_t *)(p) + 2)) << 16) | \
              ((uint64_t)(*((const uint8_t *)(p) + 3)) << 8) | \
              ((uint64_t)(*((const uint8_t *)(p) + 4)) << 0)))

#define EXTRACT_48BITS(p) \
  ((uint64_t)(((uint64_t)(*((const uint8_t *)(p) + 0)) << 40) | \
              ((uint64_t)(*((const uint8_t *)(p) + 1)) << 32) | \
              ((uint64_t)(*((const uint8_t *)(p) + 2)) << 24) | \
              ((uint64_t)(*((const uint8_t *)(p) + 3)) << 16) | \
              ((uint64_t)(*((const uint8_t *)(p) + 4)) << 8) | \
              ((uint64_t)(*((const uint8_t *)(p) + 5)) << 0)))

#define EXTRACT_56BITS(p) \
  ((uint64_t)(((uint64_t)(*((const uint8_t *)(p) + 0)) << 48) | \
              ((uint64_t)(*((const uint8_t *)(p) + 1)) << 40) | \
              ((uint64_t)(*((const uint8_t *)(p) + 2)) << 32) | \
              ((uint64_t)(*((const uint8_t *)(p) + 3)) << 24) | \
              ((uint64_t)(*((const uint8_t *)(p) + 4)) << 16) | \
              ((uint64_t)(*((const uint8_t *)(p) + 5)) << 8) | \
              ((uint64_t)(*((const uint8_t *)(p) + 6)) << 0)))

#define EXTRACT_LE_8BITS(p) (*(p))
#define EXTRACT_LE_16BITS(p) \
  ((uint16_t)(((uint16_t)(*((const uint8_t *)(p) + 1)) << 8) | \
              ((uint16_t)(*((const uint8_t *)(p) + 0)) << 0)))
#define EXTRACT_LE_32BITS(p) \
  ((uint32_t)(((uint32_t)(*((const uint8_t *)(p) + 3)) << 24) | \
              ((uint32_t)(*((const uint8_t *)(p) + 2)) << 16) | \
              ((uint32_t)(*((const uint8_t *)(p) + 1)) << 8) | \
              ((uint32_t)(*((const uint8_t *)(p) + 0)) << 0)))
#define EXTRACT_LE_24BITS(p) \
  ((uint32_t)(((uint32_t)(*((const uint8_t *)(p) + 2)) << 16) | \
              ((uint32_t)(*((const uint8_t *)(p) + 1)) << 8) | \
              ((uint32_t)(*((const uint8_t *)(p) + 0)) << 0)))
#define EXTRACT_LE_64BITS(p) \
  ((uint64_t)(((uint64_t)(*((const uint8_t *)(p) + 7)) << 56) | \
              ((uint64_t)(*((const uint8_t *)(p) + 6)) << 48) | \
              ((uint64_t)(*((const uint8_t *)(p) + 5)) << 40) | \
              ((uint64_t)(*((const uint8_t *)(p) + 4)) << 32) | \
              ((uint64_t)(*((const uint8_t *)(p) + 3)) << 24) | \
              ((uint64_t)(*((const uint8_t *)(p) + 2)) << 16) | \
              ((uint64_t)(*((const uint8_t *)(p) + 1)) << 8) | \
              ((uint64_t)(*((const uint8_t *)(p) + 0)) << 0)))

#endif
