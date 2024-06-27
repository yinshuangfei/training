
DIRS = $(shell ls -d */)

PROGRAM_NAME = client
DEPENDENCIES =

CC = gcc
CFLAGS = -Wall
CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})
LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES})

COMMON =
COMM_SRCS =
COMM_OBJS := $(COMM_SRCS:%.c=%.o)
COMM_OBJS := $(foreach i,$(COMM_OBJS),$(COMMON)/$(i))

PROGRAM_SRCS = client.o
PROGRAM_OBJS := $(PROGRAM_SRCS:%.c=%.o)

all: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)
	# @valgrind --leak-check=full -s ./$(PROGRAM_NAME)


$(PROGRAM_NAME): $(PROGRAM_OBJS) $(COMM_OBJS)
	$(CC) -g -o $@ $^ $(LDFLAGS)


.PHONY:kill
kill:
	ps -ef | grep "$(PROGRAM_NAME)" | grep -v grep | awk '{print $$2}' | xargs -i kill -9 {}


.PHONY:clean
clean:
	for i in ${DIRS}; do make $@ -C $$i; done
	-rm -rf *.o *.o.d *~ a.out $(PROGRAM_NAME) $(PROGRAM_NAME).exe
