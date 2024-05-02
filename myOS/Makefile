C_SOURCES	= $(shell find . -name "*.c")
C_OBJECTS	= $(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES	= $(shell find . -name "*.s")
S_OBJECTS	= $(patsubst %.s, %.o, $(S_SOURCES))

CC			= gcc
LD			= ld
ASM			= nasm

C_FLAGS		= -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-pic -fno-builtin \
			  -fno-stack-protector -I include
LD_FLAGS	= -T scripts/kernel.ld -m elf_i386 -nostdlib
ASM_FLAGS	= -f elf -g -F stabs

QEMU		= /usr/libexec/qemu-kvm
DISK		= disk.img

OBJDIR		=./objs

all: wr_bootloader qemu
# $(S_OBJECTS)
# @echo $(S_OBJECTS)
# $(C_OBJECTS) link update_image


.PHONY: qemu
# Boot options:
#	'drives': floppy (a), hard disk (c), CD-ROM (d), network (n)
# Block device options:
#	-fda/-fdb file  use 'file' as floppy disk 0/1 image
#	-hda/-hdb file  use 'file' as IDE hard disk 0/1 image
#	-hdc/-hdd file  use 'file' as IDE hard disk 2/3 image
qemu:
	$(QEMU) -hda $(DISK) -rtc base=localtime -nographic -m 128;
# $(QEMU) -hda $(DISK) -boot c -nographic -m 10
# $(QEMU) -hdb hd.img -fda a.img -rtc base=localtime -m 128;

wr_bootloader: gen_disk auto
	dd if=$(OBJDIR)/boot.bin of=$(DISK) status=noxfer conv=notrunc

# .c.o:
# 	@echo 编译代码文件 $< ...
# 	$(CC) $(C_FLAGS) $< -o $@

# .s.o:
# 	@echo 编译汇编文件 $< ...
# 	$(ASM) $(ASM_FLAGS) $<

link:
	@echo 链接内核文件...
	$(LD) $(LD_FLAGS) $(S_OBJECTS) $(C_OBJECTS) -o hx_kernel

# .PHONY:update_image
# update_image:
# 	sudo mount floppy.img /mnt/kernel
# 	sudo cp hx_kernel /mnt/kernel/hx_kernel
# 	sleep 1
# 	sudo umount /mnt/kernel

# .PHONY:mount_image
# mount_image:
# 	sudo mount floppy.img /mnt/kernel

# .PHONY:umount_image
# umount_image:
# 	sudo umount /mnt/kernel

# .PHONY:bochs
# bochs:
# 	bochs -f tools/bochsrc.txt

# .PHONY:debug
# debug:
# 	qemu -S -s -fda floppy.img -boot a &
# 	sleep 1
# 	cgdb -x tools/gdbinit

auto:
	@./build.sh -auto

bootlodaer:
	@./build.sh -bootloader

kernel:
	@./build.sh -kernel

.PHONY: gen_disk
gen_disk:
	@if [ ! -e $(DISK) ]; then dd if=/dev/zero of=$(DISK) bs=1M count=10; fi

.PHONY: clean
clean:
	rm -fr *.o *.O objs/* *.img
	# rm -fr $(S_OBJECTS) $(C_OBJECTS) hx_kernel boot.bin
