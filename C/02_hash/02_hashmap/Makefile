
PROGRAM_NAME = program
DEPENDENCIES = glib-2.0

CC = gcc
CFLAGS = -Wall -I./include -g
CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})
LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES}) -lpthread

COMMON = ../common
COMM_SRCS =
COMM_OBJS := $(COMM_SRCS:%.c=%.o)
COMM_OBJS := $(foreach i,$(COMM_OBJS),$(COMMON)/$(i))

SERVER_SRCS = test_main.c hashmap.c
SERVER_OBJS := $(SERVER_SRCS:%.c=%.o)


all: $(PROGRAM_NAME)
	@# clear
	# ./$(PROGRAM_NAME)
	@valgrind --leak-check=full -s ./$(PROGRAM_NAME)
	@#gdb --args $(PROGRAM_NAME) -f -m / -d


$(PROGRAM_NAME): $(SERVER_OBJS) $(COMM_OBJS)
	$(CC) -g -o $@ $^ $(LDFLAGS)


.PHONY:kill
kill:
	ps -ef | grep "$(PROGRAM_NAME)" | grep -v grep | awk '{print $$2}' | xargs -i kill -9 {}


.PHONY:clean
clean:
	-rm -rf *.o *.o.d *~ a.out $(PROGRAM_NAME)
