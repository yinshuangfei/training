DISK = grub_disk.img
LOOP = /dev/loop520

all:
	/usr/libexec/qemu-kvm  -m 1024 -drive format=raw,file=/dev/nvme0n2 -nographic	

mk_boot_disk:
	@if [ ! -e $(DISK) ]; then dd if=/dev/zero of=$(DISK) bs=1M count=100 status=progress; fi

mk_fs: mk_boot_disk
	@losetup $(LOOP) $(DISK)
	@losetup -l $(LOOP)
	#@-yes | parted $(LOOP) mklabel gpt
	./mkpart.sh
	losetup -d $(LOOP)
	#@parted $(DISK) set 1 bios_grub on
	#@mkfs.ext4 -q $(DISK)
	#@mount $(DISK) /mnt
	#@mkdir /mnt/boot
	#@grub2-install --boot-directory=/mnt/boot/ --force $(DISK)

	
