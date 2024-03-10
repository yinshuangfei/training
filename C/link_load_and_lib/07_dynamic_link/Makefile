all:
	gcc -fPIC -shared -o lib.so lib.c
	gcc -o p1 p1.c ./lib.so
	gcc -o p2 p2.c ./lib.so

got:
	objdump -h lib.so
	objdump -R lib.so


interp:
	objdump -h p1
	objdump -s p1
	readelf -l p1 | grep interpreter --color


dynamic:
	readelf -d lib.so
	ldd p1
	readelf -sD lib.so


locate:
	readelf -r lib.so
	readelf -S lib.so


debug:
	LD_DEBUG=help ./p1	
	LD_DEBUG=files ./p1	


clean:
	rm *.o *.so p1 p2 a.out test -f
