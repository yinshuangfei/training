DEPENDENCIES =

NJOBS = 2

PROGRAM_NAME = main

CC = gcc
CFLAGS = -D_FILE_OFFSET_BITS=64 -I./include -g
CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})

PROGRAM_SRCS = main.o
PROGRAM_OBJS := $(PROGRAM_SRCS:%.c=%.o)

.PHONY: all
all:
	@echo "[INFO] parallel jobs: $(NJOBS)"
	@$(MAKE) -j$(NJOBS) build_all

build_all: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)

$(PROGRAM_NAME): $(PROGRAM_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)


.PHONY:clean
clean:
	-rm -rf *.o *.o.d *~ a.out $(PROGRAM_NAME) test