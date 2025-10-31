# 定义
PROGRAM_NAME = program

# 依赖定义
DEPENDENCIES =

# 源码定义
SERVER_SRCS = test_main.c
SERVER_OBJS := $(SERVER_SRCS:%.c=%.o)

# 公共代码定义
COMMON = ../common
COMM_SRCS =
COMM_OBJS := $(COMM_SRCS:%.c=%.o)
COMM_OBJS := $(foreach i,$(COMM_OBJS),$(COMMON)/$(i))

# 编译、链接标志
CC = gcc
CFLAGS = -Wall -I./include -g
CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})
LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES}) -L./ -la

CFLAGS += -D ENABLE_LOG_TO_FILE
# 相对路径的库
CFLAGS += -Wl,--rpath=./

# DO IT !!!
all: $(PROGRAM_NAME)
	@# clear
	@./$(PROGRAM_NAME)

# 调试
dbg: $(PROGRAM_NAME)
	@gdb --args $(PROGRAM_NAME) -f


liba.so: liba.c
	$(CC) -g -o $@ $^ -shared -fPIC


# 项目编译规则
$(PROGRAM_NAME): $(SERVER_OBJS) $(COMM_OBJS) liba.so
	$(CC) -g -o $@ $^ $(CFLAGS) $(LDFLAGS)


# 杀死进程
.PHONY: kill
kill:
	ps -ef | grep "$(PROGRAM_NAME)" | grep -v grep | awk '{print $$2}' | \
		xargs -i kill -9 {}

# 清空项目
.PHONY: clean
clean:
	rm -rf *.o *.o.d a.out $(PROGRAM_NAME) liba.so
