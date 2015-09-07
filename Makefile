CC=gcc
CFLAGS=-g -Wall -Wfatal-errors -std=c99 $(shell pkg-config --cflags --libs gtk+-3.0 appindicator3-0.1 libgtop-2.0)

all: Indicator-memory

Indicator-memory: Indicator-memory.c
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -f *.o Indicator-memory

install:
	install --mode=755 Indicator-memory  /usr/bin/

uninstall:
	rm /usr/bin/Indicator-memory