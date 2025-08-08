
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void utf8teststr(char *dst,int len,int ab) {
  switch(len) {
  case 1:
    if (ab == 0) { snprintf(dst,len+1,"%c",b("0000-0000")); }
    if (ab == 1) { snprintf(dst,len+1,"%c",b("0111-1111")); }
    break;
  case 2:
    if (ab == 0) { snprintf(dst,len+1,"%c%c",b("1100-0010"),b("1000-0000")); }
    if (ab == 1) { snprintf(dst,len+1,"%c%c",b("1101-1111"),b("1011-1111")); }
    break;  
  case 3:
    if (ab == 0) {
      snprintf(dst,len+1,"%c%c%c",
	       b("1110-0000"),b("1010-0000"),b("1000-0000"));
    }
    if (ab == 1) {
      snprintf(dst,len+1,"%c%c%c",
	       b("1110-1111"),b("1011-1111"),b("1011-1111"));
    }
    break;  
  case 4:
    if (ab == 0) {
      snprintf(dst,len+1,"%c%c%c%c",
	       b("1111-0000"),b("1001-0000"),b("1000-0000"),b("1000-0000"));
    }
    if (ab == 1) {
      snprintf(dst,len+1,"%c%c%c%c",
	       b("1111-0111"),b("1011-1111"),b("1011-1111"),b("1011-1111"));
    }
    break;  
  }
}

int utf8testval(int len,int ab) {
  int val=-1;
  switch(len) {
  case 1:
    if (ab == 0) { val = 0x00; break; }
    if (ab == 1) { val = 0x7F; break; }
    break;
  case 2:
    if (ab == 0) { val = 0x80; break; }
    if (ab == 1) { val = 0x7FF; break; }
    break;  
  case 3:
    if (ab == 0) { val = 0x800; break; }
    if (ab == 1) { val = 0xFFFF; break; }
    break;  
  case 4:
    if (ab == 0) { val = 0x10000; break; }
    if (ab == 1) { val = 0x1FFFFF; break; }
    break;
  }
  return val;
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

FACTS(UTF8Len5And6) {
  // Verify boundaries for extended 5- and 6-byte sequences
  unsigned len;
  char x8[16], y8[16], tmp[16];
  uint32_t xc, yc;

  // 5-byte
  len = 5;
  xc = UTF8_MINVAL[len-1];
  yc = UTF8_MAXVAL[len-1];
  FACT(len,==,utf8enclen(xc));
  utf8encval(x8, xc, len);
  FACT(len,==,utf8declen(x8, len));
  FACT(xc,==,utf8decval(x8, len));
  utf8encval(tmp, xc, len);
  FACT(strncmp(tmp, x8, len),==,0);

  FACT(len,==,utf8enclen(yc));
  utf8encval(y8, yc, len);
  FACT(len,==,utf8declen(y8, len));
  FACT(yc,==,utf8decval(y8, len));
  utf8encval(tmp, yc, len);
  FACT(strncmp(tmp, y8, len),==,0);

  // 6-byte
  len = 6;
  xc = UTF8_MINVAL[len-1];
  yc = UTF8_MAXVAL[len-1];
  FACT(len,==,utf8enclen(xc));
  utf8encval(x8, xc, len);
  FACT(len,==,utf8declen(x8, len));
  FACT(xc,==,utf8decval(x8, len));
  utf8encval(tmp, xc, len);
  FACT(strncmp(tmp, x8, len),==,0);

  FACT(len,==,utf8enclen(yc));
  utf8encval(y8, yc, len);
  FACT(len,==,utf8declen(y8, len));
  FACT(yc,==,utf8decval(y8, len));
  utf8encval(tmp, yc, len);
  FACT(strncmp(tmp, y8, len),==,0);
}

FACTS(InvalidAndCapacity) {
  // invalid leading continuation byte
  char bad1[] = {(char)0x80, 0};
  uint32_t out[4];
  FACT(utf8declen(bad1, 1),==,-1);
  FACT(utf8decode(bad1, 1, out, 4),==,1);
  FACT(out[0],==,(uint8_t)bad1[0]);

  // truncated 2-byte sequence: C2 (missing continuation)
  char bad2[] = {(char)0xC2, 0};
  FACT(utf8declen(bad2, 1),==,-1);
  FACT(utf8decode(bad2, 1, out, 4),==,1);
  FACT(out[0],==,(uint8_t)bad2[0]);

  // capacity handling: encode into too-small buffer
  uint32_t u32seq[] = {0x24, 0x20AC, 0x10348}; // '$', '€', U+10348
  char buf[8];
  int need = utf8encode(u32seq, 3, NULL, 0);
  FACT(need,==,1 + 3 + 4);
  int wrote = utf8encode(u32seq, 3, buf, 2); // only room for first codepoint
  FACT(wrote,==,need);
  FACT((unsigned char)buf[0],==,0x24);
}

FACTS(decode) {
  char u1[80],u2[80];
  uint32_t w1[80],w2[80];
  
  FACT(utf8decode("",0,NULL,0),==,0);
  for (int len=1; len<=4; ++len) {
    for (int ab=0; ab <= 1; ++ab) {
      for (int i=0; i<80; ++i) {
	u1[i]=u2[i]=-1;
	w1[i]=w2[i]=-1;
      }
      utf8teststr(u1,len,ab);
      int32_t wc32=utf8testval(len,ab);
      w1[0]=wc32;
      FACT(wc32,==,w1[0]);
      FACT(utf8decode(u1,len,NULL,0),==,1);
      FACT(utf8encode(w1,1,NULL,0),==,len);
      utf8decode(u1,len,w2,1);
      utf8encode(w1,1,u2,len);
      FACT(w1[0],==,w2[0]);
      for (int i=0; i<len; ++i) FACT(u1[i],==,u2[i]);
    }
  }
  for (int len1=1; len1<=4; ++len1) {
    for (int ab1=0; ab1 <= 1; ++ab1) {
      for (int len2=1; len2<=4; ++len2) {
	for (int ab2=0; ab2 <= 1; ++ab2) {
	  for (int i=0; i<80; ++i) {
	    u1[i]=u2[i]=-1;
	    w1[i]=w2[i]=-1;
	  }
	  utf8teststr(u1,len1,ab1);
	  utf8teststr(u1+len1,len2,ab2);
	  w1[0]=utf8testval(len1,ab1);
	  w1[1]=utf8testval(len2,ab2);
	  FACT(utf8decode(u1,len1+len2,NULL,0),==,2);
	  FACT(utf8encode(w1,2,NULL,0),==,len1+len2);
	  utf8decode(u1,len1+len2,w2,2);
	  utf8encode(w1,2,u2,len1+len2);
	  FACT(w1[0],==,w2[0]);
	  FACT(w1[1],==,w2[1]);	  
	  for (int i=0; i<len1+len2; ++i) FACT(u1[i],==,u2[i]);
	}
      }
    }
  }
}



FACTS(Suits) {
  uint32_t wc [] = {0x2663, 0x2666, 0x2665, 0x2660};  
  for (int i=0; i<4; ++i) {
    char buf[16];
    int utf8len = utf8enclen(wc[i]);
    utf8encval(buf,wc[i],utf8len);
    int utf8len2=utf8declen(buf,utf8len);
    FACT(utf8len2,==,utf8len);
    int wc2=utf8decval(buf,utf8len); 
    FACT(wc2,==,wc[i]);
    buf[utf8len]=0;
  }
}

FACTS_REGISTER_ALL() {
    FACTS_REGISTER(UTF8Len1);
    FACTS_REGISTER(UTF8Len2);
    FACTS_REGISTER(UTF8Len3);
    FACTS_REGISTER(UTF8Len4);
    FACTS_REGISTER(decode);
    FACTS_REGISTER(Suits);
    FACTS_REGISTER(InvalidAndCapacity);
    FACTS_REGISTER(UTF8Len5And6);
}

FACTS_MAIN
