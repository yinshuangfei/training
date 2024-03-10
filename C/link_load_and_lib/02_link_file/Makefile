all:
	#gcc -c a.c b.c
	gcc -c a.c -fno-stack-protector -o a.o
	gcc -c b.c
	ld a.o b.o -e main -o ab


show_static_lib:
	@echo "显示静态链接库中包含的目标文件："
	ar -t /usr/lib/x86_64-linux-gnu/libc.a | head
	@echo "\n查找printf函数："
	objdump -t /usr/lib/x86_64-linux-gnu/libc.a | grep " printf" --color
	
