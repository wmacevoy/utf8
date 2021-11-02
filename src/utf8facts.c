#include <stdlib.h>
#include <stdio.h>

#include "facts.h"
#include "utf8.h"

unsigned char b(const char *str) {
  unsigned char x = 0;
  while (*str != 0) {
    if (*str == '0' || *str == '1') {
      x = (x << 1) | (*str == '1');
    }
    ++str;
  }
  return x;
}

FACTS(UTF8Len1) {
  unsigned xc,yc,len;
  char x8[1024],xtmp[1024],y8[1024],ytmp[1024];

  xc=0; yc=0x7F;
  snprintf(x8,sizeof(x8),"%c",b("0000-0000"));
  snprintf(y8,sizeof(y8),"%c",b("0111-1111"));

  len = 1;
  FACT(xc,==,UTF8_MINVAL[len-1]);
  if (len > 1) FACT(xc,==,UTF8_MAXVAL[len-2]+1);
  FACT(yc,==,UTF8_MAXVAL[len-1]);
  if (len < 4) FACT(yc,==,UTF8_MINVAL[len]-1);
  FACT(len,==,utf8declen(x8,len));
  FACT(-1,==,utf8declen(x8,len-1));
  FACT(xc,==,utf8decval(x8,len));
  FACT(len,==,utf8enclen(xc));
  utf8encval(xtmp,xc,len);
  FACT(strncmp(xtmp,x8,len),==,0);
  
  FACT(len,==,utf8declen(y8,len));
  FACT(-1,==,utf8declen(y8,len-1));  
  FACT(yc,==,utf8decval(y8,len));
  FACT(len,==,utf8enclen(yc));
  utf8encval(ytmp,yc,len);
  FACT(strncmp(ytmp,y8,len),==,0);
}

FACTS(UTF8Len2) {

  unsigned xc,yc,len;
  char x8[1024],xtmp[1024],y8[1024],ytmp[1024];

  xc=0x80; yc=0x7FF;
  snprintf(x8,sizeof(x8),"%c%c",b("1100-0010"),b("1000-0000"));
  snprintf(y8,sizeof(y8),"%c%c",b("1101-1111"),b("1011-1111"));

  len = 2;
  FACT(xc,==,UTF8_MINVAL[len-1]);
  if (len > 1) FACT(xc,==,UTF8_MAXVAL[len-2]+1);
  FACT(yc,==,UTF8_MAXVAL[len-1]);
  if (len < 4) FACT(yc,==,UTF8_MINVAL[len]-1);
  FACT(len,==,utf8declen(x8,len));
  FACT(-1,==,utf8declen(x8,len-1));
  FACT(xc,==,utf8decval(x8,len));
  FACT(len,==,utf8enclen(xc));
  utf8encval(xtmp,xc,len);
  FACT(strncmp(xtmp,x8,len),==,0);
  
  FACT(len,==,utf8declen(y8,len));
  FACT(-1,==,utf8declen(y8,len-1));  
  FACT(yc,==,utf8decval(y8,len));
  FACT(len,==,utf8enclen(yc));
  utf8encval(ytmp,yc,len);
  FACT(strncmp(ytmp,y8,len),==,0);
}

FACTS(UTF8Len3) {
  unsigned xc,yc,len;
  char x8[1024],xtmp[1024],y8[1024],ytmp[1024];

  xc=0x800; yc=0xFFFF;
  snprintf(x8,sizeof(x8),"%c%c%c",
	   b("1110-0000"),b("1010-0000"),b("1000-0000"));
  snprintf(y8,sizeof(y8),"%c%c%c",
	   b("1110-1111"),b("1011-1111"),b("1011-1111"));

  len = 3;
  FACT(xc,==,UTF8_MINVAL[len-1]);
  if (len > 1) FACT(xc,==,UTF8_MAXVAL[len-2]+1);
  FACT(yc,==,UTF8_MAXVAL[len-1]);
  if (len < 4) FACT(yc,==,UTF8_MINVAL[len]-1);
  FACT(len,==,utf8declen(x8,len));
  FACT(-1,==,utf8declen(x8,len-1));
  FACT(xc,==,utf8decval(x8,len));
  FACT(len,==,utf8enclen(xc));
  utf8encval(xtmp,xc,len);
  FACT(strncmp(xtmp,x8,len),==,0);
  
  FACT(len,==,utf8declen(y8,len));
  FACT(-1,==,utf8declen(y8,len-1));  
  FACT(yc,==,utf8decval(y8,len));
  FACT(len,==,utf8enclen(yc));
  utf8encval(ytmp,yc,len);
  FACT(strncmp(ytmp,y8,len),==,0);
}

FACTS(UTF8Len4) {
  unsigned xc,yc,len;
  char x8[1024],xtmp[1024],y8[1024],ytmp[1024];

  xc=0x10000; yc=0x1FFFFF;
  snprintf(x8,sizeof(x8),"%c%c%c%c",
	   b("1111-0000"),b("1001-0000"),b("1000-0000"),b("1000-0000"));
  snprintf(y8,sizeof(y8),"%c%c%c%c",
	   b("1111-0111"),b("1011-1111"),b("1011-1111"),b("1011-1111"));

  len = 4;
  FACT(xc,==,UTF8_MINVAL[len-1]);
  if (len > 1) FACT(xc,==,UTF8_MAXVAL[len-2]+1);
  FACT(yc,==,UTF8_MAXVAL[len-1]);
  if (len < 4) FACT(yc,==,UTF8_MINVAL[len]-1);
  FACT(len,==,utf8declen(x8,len));
  FACT(-1,==,utf8declen(x8,len-1));
  FACT(xc,==,utf8decval(x8,len));
  FACT(len,==,utf8enclen(xc));
  utf8encval(xtmp,xc,len);
  FACT(strncmp(xtmp,x8,len),==,0);
  
  FACT(len,==,utf8declen(y8,len));
  FACT(-1,==,utf8declen(y8,len-1));  
  FACT(yc,==,utf8decval(y8,len));
  FACT(len,==,utf8enclen(yc));
  utf8encval(ytmp,yc,len);
  FACT(strncmp(ytmp,y8,len),==,0);
}

FACTS_FINISHED
FACTS_MAIN







