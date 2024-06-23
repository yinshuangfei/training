
PROGRAM_NAME = server

DEPENDENCIES =

CC = gcc
CFLAGS = -Wall
CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})
LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES}) -lpthread

PROGRAM_SRCS = server.o
PROGRAM_OBJS := $(PROGRAM_SRCS:%.c=%.o)

all: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)
	# @valgrind --leak-check=full -s ./$(PROGRAM_NAME)


$(PROGRAM_NAME): $(PROGRAM_OBJS)
	$(CC) -g -o $@ $^ $(LDFLAGS)


.PHONY:kill
kill:
	ps -ef | grep "$(PROGRAM_NAME)" | grep -v grep | awk '{print $$2}' | xargs -i kill -9 {}


.PHONY:clean
clean:
	-rm -rf *.o *.o.d *~ a.out $(PROGRAM_NAME)
