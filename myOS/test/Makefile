
DEPENDENCIES =

CC = gcc
CFLAGS = -D_FILE_OFFSET_BITS=64 -I./include -g
CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})
LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES})

AR = ar
ARFLAGS = crs

COMMON = ../common
COMM_SRCS =
COMM_OBJS := $(COMM_SRCS:%.c=%.o)
COMM_OBJS := $(foreach i,$(COMM_OBJS),$(COMMON)/$(i))

CLIENT_SRCS = test_pwrite.o
CLIENT_OBJS := $(CLIENT_SRCS:%.c=%.o)

all: test


test: $(COMM_OBJS) $(CLIENT_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	./$@


.PHONY:clean
clean:
	-rm -rf *.o *.o.d *~ a.out client test