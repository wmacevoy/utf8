#pragma once
// input
//
//   utf8 - encoded string of length n (may contain 0's)
//   n    - length of utf8 string
//
// return value
//
//   1-4 length of utf8-encoded codepoint as first character of utf8
//   -1  invalid encoding (or n <= 0)

#include <stddef.h>
#include <stdint.h>

#ifndef UTF8_C
extern uint32_t UTF8_MINVAL[4];
extern uint32_t UTF8_MAXVAL[4];
#endif

int utf8declen(const unsigned char *utf8, int n);
int32_t utf8decval(const unsigned char *utf8, int len);
int utf8enclen(unsigned val);
void utf8encval(unsigned char *utf8, uint32_t val, int enclen);

int utf8decode(const unsigned char *utf8, int utf8len, uint32_t *wc, int wccap);
int utf8encode(const uint32_t *wc, int wclen, unsigned char *utf8, int utf8cap);


