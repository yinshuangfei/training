LINKFLAGS = $(shell pkg-config --cflags  --libs glib-2.0)

all:
	gcc hash_glib.c  $(LINKFLAGS)

.PHONY: clean
clean:
	rm -f a.out *.o
