# 制作 img 文件使用 bximage 工具

DISK = ../disk_grub.img
#DISK = ../disk.img

all: run

show_disk_content:
	@hexdump -C -n 512 $(DISK)

run:
	@bochsdbg -q -f bochsrc.bxrc
