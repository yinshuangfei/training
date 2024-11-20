DEPENDENCIES = glib-2.0

CC = gcc
CFLAGS = -Wall -D_FILE_OFFSET_BITS=64 -I./include -g
CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})
LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES})

COMMON = ../common
COMM_SRCS =
COMM_OBJS_TMP := $(COMM_SRCS:%.c=%.o)
COMM_OBJS := $(foreach i,$(COMM_OBJS_TMP),$(COMMON)/$(i))

SERVER_SRCS = test_main.o rbtree.o
SERVER_OBJS := $(SERVER_SRCS:%.c=%.o)


.PHONY: all
all:
	@#echo "[INFO] parallel jobs: $(NJOBS)"
	@$(MAKE) -j$(NJOBS) server
	@./server


server: $(SERVER_OBJS) $(COMM_OBJS)
	$(CC) -g -o $@ $^ $(LDFLAGS) -lpthread


.PHONY:kill
kill:
	ps -ef | grep "server dd" | grep -v grep | awk '{print $$2}' | \
	xargs -i kill -9 {}


.PHONY:clean
clean:
	-rm -rf *.o *.o.d *~ a.out server
