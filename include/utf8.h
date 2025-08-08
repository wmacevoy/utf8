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
extern const uint32_t UTF8_MINVAL[6];
extern const uint32_t UTF8_MAXVAL[6];
#endif

int utf8declen(const char *utf8, int n);
int32_t utf8decval(const char *utf8, int len);
int utf8enclen(uint32_t val);          // returns 1..6
void utf8encval(char *utf8, uint32_t val, int enclen);

int utf8decode(const char *utf8, int utf8len, uint32_t *u32, int u32cap);
int utf8encode(const uint32_t *u32, int u32len, char *utf8, int utf8cap);


