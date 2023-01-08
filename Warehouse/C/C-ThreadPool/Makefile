#all:
#	gcc main.c pool.c -o app -lpthread


CC = gcc
CFLAGS = -g -Wall -o2
LIB = -lpthread

object = main.o pool.o
exe = app

RUNE = $(CC) $(CFLAGS) $(object) -o $(exe) $(LIB)
RUNO = $(CC) $(CFLAGS) -c $< -o $@ $(LIB)

.RHONY:clean

$(exe):$(object)
	$(RUNE)

%.o:%.c pool.h
	$(RUNO)
%.o:%.c
	$(RUNO)


clean:
	-rm -rf *.o app *~
