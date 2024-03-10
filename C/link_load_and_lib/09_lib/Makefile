all: show_crti show_crtn

show_crti:
	objdump -dr /usr/lib/x86_64-linux-gnu/crti.o

show_crtn:
	objdump -dr /usr/lib/x86_64-linux-gnu/crtn.o

syscall:
	ldd /usr/bin/cat
	cat /proc/self/maps | grep vdso

clean:
	rm *.o *.so p1 p2 a.out test -f
