# 定义
PROGRAM_NAME1 = program1
PROGRAM_NAME2 = program2
PROGRAM_NAME3 = program3

# 依赖定义
DEPENDENCIES =

# 源码定义
SERVER_SRCS = iniparser.c dictionary.c
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
LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES})

# CFLAGS += -D ENABLE_LOG_TO_FILE

# DO IT !!!
all: $(PROGRAM_NAME1)
	@# clear
	@./$(PROGRAM_NAME1)


# 项目编译规则
$(PROGRAM_NAME1): $(SERVER_OBJS) $(COMM_OBJS) test_main_01.c
	$(CC) -g -o $@ $^ $(LDFLAGS)


$(PROGRAM_NAME2): $(SERVER_OBJS) $(COMM_OBJS) test_main_02.c
	$(CC) -g -o $@ $^ $(LDFLAGS)


$(PROGRAM_NAME3): $(SERVER_OBJS) $(COMM_OBJS) test_main_03.c
	$(CC) -g -o $@ $^ $(LDFLAGS)


# 清空项目
.PHONY: clean
clean:
	rm -rf *.o *.o.d a.out $(PROGRAM_NAME1) \
	$(PROGRAM_NAME2) $(PROGRAM_NAME3) example.ini
