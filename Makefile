CFLAGS=-g -Iinclude -I../facts/include
LDLIBS=-lm

all : bin/utf8_facts

.PHONY: check
check : all
	bin/utf8_facts | diff - utf8_facts.expected

bin/utf8_facts : src/utf8_facts.c src/utf8.c include/utf8.h
	mkdir -p bin
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/utf8_facts.c src/utf8.c ../facts/src/facts.c $(LDLIBS)
