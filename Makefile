# Makefile for flexible checksum utility

SOURCES := $(wildcard *.c) $(wildcard methods/*.c)
OBJS    := $(patsubst %.c,%.o,$(SOURCES))

APP := checksum

CC    := gcc
COPTS := -Wall -I.

default: $(APP)
all: $(APP)

$(APP): $(OBJS)
	$(CC) $(COPTS) -o $@ $^

%.o: %.c
	$(CC) $(COPTS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(APP)

rebuild: clean all
.PHONY: rebuild clean all default

