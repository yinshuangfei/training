# 定义
PROGRAM_NAME1 = server
PROGRAM_NAME2 = client

# 依赖定义
DEPENDENCIES =

# 源码定义
SERVER_SRCS = test_server.c
SERVER_OBJS := $(SERVER_SRCS:%.c=%.o)

CLIENT_SRCS = test_client.c
CLIENT_OBJS := $(CLIENT_SRCS:%.c=%.o)

# 公共代码定义
COMMON = ../common
COMM_SRCS =
COMM_OBJS := $(COMM_SRCS:%.c=%.o)
COMM_OBJS := $(foreach i,$(COMM_OBJS),$(COMMON)/$(i))

# 编译、链接标志
CC = gcc
CFLAGS = -Wall -I./include -g
CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})
LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES})

CFLAGS += -D ENABLE_LOG_TO_FILE

# DO IT !!!
all: $(PROGRAM_NAME1) $(PROGRAM_NAME2)
	@# clear


# 调试
dbg: $(PROGRAM_NAME1)
	@gdb --args $(PROGRAM_NAME1) -f


# 项目编译规则
$(PROGRAM_NAME1): $(SERVER_OBJS) $(COMM_OBJS)
	$(CC) -g -o $@ $^ $(LDFLAGS)


# 项目编译规则
$(PROGRAM_NAME2): $(CLIENT_OBJS) $(COMM_OBJS)
	$(CC) -g -o $@ $^ $(LDFLAGS)


# 杀死进程
.PHONY: kill
kill:
	ps -ef | grep "$(PROGRAM_NAME1)" | grep -v grep | awk '{print $$2}' | \
		xargs -i kill -9 {}


# 清空项目
.PHONY: clean
clean:
	rm -rf *.o *.o.d a.out $(PROGRAM_NAME1) $(PROGRAM_NAME2)
