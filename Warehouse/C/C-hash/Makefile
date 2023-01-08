DEPENDENCIES = glib-2.0
LDFLAGS += $(shell pkg-config --cflags --libs ${DEPENDENCIES})

all: 
	$(CC) -o test hash.c $(LDFLAGS)
	./test