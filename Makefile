CFLAGS=-g -Iinclude -I../facts/include
LDLIBS=-lm

all : bin/utf8facts

bin/utf8facts : src/utf8facts.c src/utf8.c include/utf8.h
	mkdir -p bin
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/utf8facts.c src/utf8.c ../facts/src/facts.c $(LDLIBS)
