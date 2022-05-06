DEPENDENCIES = glib-2.0

CC = gcc
CFLAGS = -D_FILE_OFFSET_BITS=64 -I./include 
CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})

# LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES}) -lenet -pthread -levent
LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES}) -pthread -levent

all: clean server client
	clear
	./server

server: server.o test_server.o protocol.o list.o ring_buffer.o
	$(CC) -o $@ $^ $(LDFLAGS) 

client: client.o test_client.o protocol.o list.o ring_buffer.o
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY:clean
clean:
	-rm -rf *.o *.o.d *~ a.out server client
