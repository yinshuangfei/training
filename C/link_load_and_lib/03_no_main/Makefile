all: nomain bfd


nomain: 
	gcc -c -m32 -fno-builtin nomain.c
	ld -T nomain.lds -static -m elf_i386 -e nomain -o nomain nomain.o


bfd:
	gcc -o target target.c -lbfd


clean:
	rm -f nomain target *.o
