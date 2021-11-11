#define UTF8_C
#include "utf8.h"


unsigned UTF8_MINVAL[4] = { 0x00, 0x80,   0x800,  0x10000 };
unsigned UTF8_MAXVAL[4] = { 0x7F, 0x7FF, 0xFFFF, 0x1FFFFF };

int utf8declen(const unsigned char *utf8, int n) {
  if (n <= 0) return -1;
  unsigned char c = utf8[0];
  if ((c & 0x80) == 0) return 1;
  if ((c & 0x40) != 0) {
    for (int len=2; len <= 4; ++len) {
      if (n < len || ((utf8[len-1] & 0xC0) != 0x80)) return -1;

      if ((c & (0x80 >> len)) == 0) return len;
    }
  }
  return -1;
}

int utf8decval(const unsigned char *utf8, int len) {
  if (len == 1) {
    return utf8[0];
  } else if (len > 0) {
    unsigned val = utf8[0] & ~(0xFF << (7-len));
    for (int i=1; i<len; ++i) {
      val = (val  << 6) | (utf8[i] & 0x3F);
    }
    return val;
  }
  return -1;
}

int utf8enclen(unsigned val) {
  if (val < 0x80) {
    return 1;
  } else if (val < 0x0800) {
    return 2;
  } else if (val < 0x10000) {
    return 3;
  }
  return 4;
}

static unsigned char utf8encmask[] = {0x00,0xC0,0xE0,0xF0};

void utf8encval(unsigned char *utf8, unsigned val, int enclen) {
  for (int i=enclen-1; i>0; --i) {
    utf8[i]=(0x80)|(val&0x3F);
    val >>= 6;
  }
  utf8[0]=utf8encmask[enclen-1]|val;
}

int utf8codecount(const unsigned char *utf8, int utf8len) {
  return utf8codewrite(utf8,utf8len,NULL,0);
}

int utf8codewrite(const unsigned char *utf8, int utf8len, wchar_t *dest, int destlen) {
  int ans = 0;
  while (utf8len > 0) {
    int declen=utf8declen(utf8,utf8len);
    if (declen > 0) {
      if (destlen > 0) {
	dest[0]=utf8decval(utf8,declen);
	--destlen;
	++dest;
      }
      utf8 += declen;
      utf8len -= declen;
      ans += 1;
    } else {
      if (destlen > 0) {
	dest[0]=utf8[0];
	--destlen;
	++dest;
      }
      utf8 += 1;
      utf8len -= 1;
      ans += 1;
    }
  }
  return ans;
}
