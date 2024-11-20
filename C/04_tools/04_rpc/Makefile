# 定义
SERVER_PROGRAM_NAME = server
CLIENT_PROGRAM_NAME = client

# 依赖定义
DEPENDENCIES =

# 源码定义
SERVER_SRCS = server_main.c
SERVER_OBJS := $(SERVER_SRCS:%.c=%.o)

CLIENT_SRCS = client_main.c
CLIENT_OBJS := $(CLIENT_SRCS:%.c=%.o)

# 公共代码定义
COMMON = ./
COMM_SRCS = tcp_rw.c
COMM_OBJS := $(COMM_SRCS:%.c=%.o)
COMM_OBJS := $(foreach i,$(COMM_OBJS),$(COMMON)/$(i))

# 编译、链接标志
CC = gcc
CFLAGS = -Wall -D_FILE_OFFSET_BITS=64 -I./include -g
CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})
LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES}) -lpthread

CFLAGS = -D ENABLE_LOG_TO_FILE

# DO IT !!!
all: $(SERVER_PROGRAM_NAME) $(CLIENT_PROGRAM_NAME)
	@# clear
	@./$(SERVER_PROGRAM_NAME)


# 项目编译规则
$(SERVER_PROGRAM_NAME): $(SERVER_OBJS) $(COMM_OBJS) server.o
	$(CC) -g -o $@ $^ $(LDFLAGS)

$(CLIENT_PROGRAM_NAME): $(CLIENT_OBJS) $(COMM_OBJS)
	$(CC) -g -o $@ $^ $(LDFLAGS)

# 杀死进程
.PHONY: kill
kill:
	ps -ef | grep "$(SERVER_PROGRAM_NAME)" | grep -v grep | awk '{print $$2}' | \
		xargs -i kill -9 {}

# 清空项目
.PHONY: clean
clean:
	rm -rf *.o *.o.d a.out $(SERVER_PROGRAM_NAME) $(CLIENT_PROGRAM_NAME)
