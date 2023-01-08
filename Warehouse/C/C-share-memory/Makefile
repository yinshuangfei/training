
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $< -MMD -MT $@ -MF $@.d

all: clean client_1 client_2

client_1: client1.o
	$(CC) -o $@ $^ $(LDFLAGS)

client_2: client2.o
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *.o *.o.d client_1 client_2
