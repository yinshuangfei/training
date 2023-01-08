
CC = gcc
LDFLAGS += -pthread

all: clean serial
	./serial

serial: main_serial.o comm.o cmd.o
	$(CC) -o $@ $^ $(LDFLAGS)  

.PHONY: clean
clean:
	rm -f *.o serial 
