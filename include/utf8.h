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

#ifndef UTF8_C
extern unsigned UTF8_MINVAL[4];
extern unsigned UTF8_MAXVAL[4];
#endif

int utf8declen(const unsigned char *utf8, int n);
int utf8decval(const unsigned char *utf8, int len);
int utf8enclen(unsigned val);
void utf8encval(unsigned char *utf8, unsigned val, int enclen);

int utf8decode(const unsigned char *utf8, int utf8len, wchar_t *wc, int wccap);
int utf8encode(const wchar_t *wc, int wclen, unsigned char *utf8, int utf8cap);


