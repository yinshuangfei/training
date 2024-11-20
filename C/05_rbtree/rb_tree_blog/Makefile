CC = gcc

all: main.o rb_tree.o
	$(CC) $^ -o app
	./app

.PHONY: clean
clean:
	rm *.o 