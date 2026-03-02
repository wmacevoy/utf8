VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_PATCH = 0
VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

PREFIX    ?= /usr/local
LIBDIR    ?= $(PREFIX)/lib
INCLUDEDIR?= $(PREFIX)/include
PKGCONFIGDIR ?= $(LIBDIR)/pkgconfig

CC       ?= cc
CFLAGS   ?= -g
CFLAGS   += -Iinclude
LDLIBS   = -lm

UNAME_S := $(shell uname -s)

# Library targets
STATIC_LIB = libutf8.a
ifeq ($(UNAME_S),Darwin)
  SHARED_LIB = libutf8.$(VERSION_MAJOR).dylib
  SONAME_FLAG = -install_name,$(LIBDIR)/$(SHARED_LIB)
  SOLINK     = libutf8.dylib
else
  SHARED_LIB = libutf8.so.$(VERSION)
  SONAME     = libutf8.so.$(VERSION_MAJOR)
  SONAME_FLAG = -soname,$(SONAME)
  SOLINK     = libutf8.so
endif

LIB_SRC = src/utf8.c
LIB_OBJ = src/utf8.o
LIB_PIC = src/utf8.pic.o

# Test target
TEST_BIN = bin/utf8_facts
TEST_SRC = src/utf8_facts.c
FACTS_SRC = vendor/facts/src/facts.c
TEST_CFLAGS = $(CFLAGS) -Ivendor/facts/include

all: $(STATIC_LIB) $(SHARED_LIB)

$(LIB_OBJ): $(LIB_SRC) include/utf8.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(LIB_PIC): $(LIB_SRC) include/utf8.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

$(STATIC_LIB): $(LIB_OBJ)
	$(AR) rcs $@ $<

$(SHARED_LIB): $(LIB_PIC)
	$(CC) -shared -Wl,$(SONAME_FLAG) -o $@ $< $(LDLIBS)

$(TEST_BIN): $(TEST_SRC) $(STATIC_LIB) include/utf8.h
	mkdir -p bin
	$(CC) $(TEST_CFLAGS) $(LDFLAGS) -o $@ $(TEST_SRC) $(FACTS_SRC) $(STATIC_LIB) $(LDLIBS)

.PHONY: check
check: $(TEST_BIN)
	$(TEST_BIN) | diff - expected/utf8_facts.out

.PHONY: expected
expected: $(TEST_BIN)
	$(TEST_BIN) >expected/utf8_facts.out || true

utf8.pc: utf8.pc.in
	sed -e 's|@PREFIX@|$(PREFIX)|g' \
	    -e 's|@LIBDIR@|$(LIBDIR)|g' \
	    -e 's|@INCLUDEDIR@|$(INCLUDEDIR)|g' \
	    -e 's|@PROJECT_VERSION@|$(VERSION)|g' \
	    $< >$@

.PHONY: install
install: $(STATIC_LIB) $(SHARED_LIB) utf8.pc
	install -d $(DESTDIR)$(INCLUDEDIR)
	install -d $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)$(PKGCONFIGDIR)
	install -m 644 include/utf8.h $(DESTDIR)$(INCLUDEDIR)/utf8.h
	install -m 644 $(STATIC_LIB) $(DESTDIR)$(LIBDIR)/$(STATIC_LIB)
	install -m 755 $(SHARED_LIB) $(DESTDIR)$(LIBDIR)/$(SHARED_LIB)
ifneq ($(UNAME_S),Darwin)
	ln -sf $(SHARED_LIB) $(DESTDIR)$(LIBDIR)/$(SONAME)
endif
	ln -sf $(SHARED_LIB) $(DESTDIR)$(LIBDIR)/$(SOLINK)
	install -m 644 utf8.pc $(DESTDIR)$(PKGCONFIGDIR)/utf8.pc

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(INCLUDEDIR)/utf8.h
	rm -f $(DESTDIR)$(LIBDIR)/$(STATIC_LIB)
	rm -f $(DESTDIR)$(LIBDIR)/$(SHARED_LIB)
ifneq ($(UNAME_S),Darwin)
	rm -f $(DESTDIR)$(LIBDIR)/$(SONAME)
endif
	rm -f $(DESTDIR)$(LIBDIR)/$(SOLINK)
	rm -f $(DESTDIR)$(PKGCONFIGDIR)/utf8.pc

.PHONY: clean
clean:
	rm -f src/*.o $(STATIC_LIB) $(SHARED_LIB) libutf8.so* libutf8*.dylib utf8.pc
	rm -rf bin
