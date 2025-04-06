# 定义
PROGRAM_NAME = program

# 依赖定义
DEPENDENCIES =

# 源码定义
SERVER_SRCS = \
	00_load_data/load_random.c \
	01_sort/insert_sort.c \
	01_sort/shell_sort.c \
	01_sort/bubble_sort.c\
	test_main.c
SERVER_OBJS := $(SERVER_SRCS:%.c=%.o)

# 公共代码定义
COMMON = ../common
COMM_SRCS =
COMM_OBJS := $(COMM_SRCS:%.c=%.o)
COMM_OBJS := $(foreach i,$(COMM_OBJS),$(COMMON)/$(i))

# 编译、链接标志
CC = gcc
CFLAGS = -Wall -D_FILE_OFFSET_BITS=64 -I./include -g
CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})
LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES})

CFLAGS += -D ENABLE_LOG_TO_FILE

# DO IT !!!
# valgrind --leak-check=full ./$(PROGRAM_NAME)
all: $(PROGRAM_NAME)
	@# clear
	@./$(PROGRAM_NAME)

# 调试
dbg: $(PROGRAM_NAME)
	@gdb --args $(PROGRAM_NAME) -f


# 项目编译规则
$(PROGRAM_NAME): $(SERVER_OBJS) $(COMM_OBJS)
	$(CC) -g -o $@ $^ $(LDFLAGS)


# 杀死进程
.PHONY: kill
kill:
	ps -ef | grep "$(PROGRAM_NAME)" | grep -v grep | awk '{print $$2}' | \
		xargs -i kill -9 {}

# 清空项目
.PHONY: clean
clean:
	rm -rf *.o *.o.d a.out $(PROGRAM_NAME) data.txt
	make -C 00_load_data clean
	make -C 01_sort clean
