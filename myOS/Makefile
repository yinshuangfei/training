QEMU		= /usr/libexec/qemu-kvm
DISK		= disk.img
OBJDIR		= ./objs
KERNEL		= $(OBJDIR)/kernel.grub.bin

all: auto wr_bootloader qemu

auto_grub_kernel:
	@make KERNEL="$(KERNEL)" -C grub/

.PHONY: qemu
# Boot options:
#	'drives': floppy (a), hard disk (c), CD-ROM (d), network (n)
# Block device options:
#	-fda/-fdb file  use 'file' as floppy disk 0/1 image
#	-hda/-hdb file  use 'file' as IDE hard disk 0/1 image
#	-hdc/-hdd file  use 'file' as IDE hard disk 2/3 image
# -fda: 读取软盘, -hda: 读取硬盘
qemu:
	$(QEMU) -hda $(DISK) -rtc base=localtime -nographic -m 128;
# $(QEMU) -hda $(DISK) -boot c -nographic -m 10
# $(QEMU) -hdb hd.img -fda a.img -rtc base=localtime -m 128;

auto:
	@./build.sh -auto

wr_bootloader: gen_disk
	dd if=tmp_$(DISK) of=$(DISK) status=noxfer conv=notrunc

.PHONY: gen_disk
gen_disk:
	@if [ ! -e $(DISK) ]; then dd if=/dev/zero of=$(DISK) bs=1M count=10; fi

# .PHONY:debug
# debug:
# 	qemu -S -s -fda floppy.img -boot a &
# 	sleep 1
# 	cgdb -x tools/gdbinit
#

.PHONY: clean
clean:
	rm -fr *.o *.O objs/* *.img grub/*.img
