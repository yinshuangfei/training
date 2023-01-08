CC = gcc
CFLAGS = -D_FILE_OFFSET_BITS=64 -I./include 
CFLAGS += $(shell pkg-config --cflags )
LDFLAGS += -lpthread

all: s c

s: server.o
	$(CC) -o $@ $^ $(LDFLAGS) 
	./$@ 

c: client.o
	$(CC) -o $@ $^ $(LDFLAGS) 
	./$@

.PHONY: clean
clean:
	rm -fr server client *.o
