CFLAGS=-g -Iinclude -I../facts/include
LDLIBS=-lm

bin/utf8facts : src/utf8facts.c src/utf8.c include/utf8.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/utf8facts.c src/utf8.c ../facts/src/facts.c $(LDLIBS)

