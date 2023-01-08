DEPENDENCIES=glib-2.0

CFLAGS += $(shell pkg-config --cflags ${DEPENDENCIES})

LDFLAGS += $(shell pkg-config --libs ${DEPENDENCIES})

all: g_thread_test.o
	$(CC) -o test $^ $(LDFLAGS)
