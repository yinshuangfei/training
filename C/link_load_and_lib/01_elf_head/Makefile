all: show

demo: demo.c
	gcc $< -c $@.o
	gcc $< -o $@


.PHOMY: show
show: demo
	@echo "ELF文件的样例描述如下："
	@file $< | grep executable --color
	@file $<.o | grep relocatable --color 
	@file /usr/libexec/sudo/group_file.so | grep shared --color
	@echo "\n\n\n目标文件的头部："
	@objdump -x $<.o 
	@echo "\n\n\nELF目标文件的段信息："
	@size $<.o
	@echo "\n\n\nELF目标文件的段内容信息和反汇编信息："
	@objdump -s -d $<.o


show_head: demo
	@echo "ELF 目标文件的文件头全部内容："
	@readelf -h $<.o
	@echo "ELF 目标文件的字节内容："
	@hexdump -C $<.o | head
	

show_section: demo
	@echo "目标文件的段表全部内容："
	@readelf -S $<.o


show_symbolic_table: demo
	@echo "ELF符号表中的全局符号："
	@nm $<.o
	@echo "ELF符号表内容："
	@readelf -s $<.o


.PHOMY: clean
clean:
	rm -f *.o demo
