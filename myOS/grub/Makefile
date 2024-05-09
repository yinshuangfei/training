DISK = grub_disk.img
LOOP = /dev/loop520
LOOP_PART= /dev/mapper/loop520p1
MNT = /mnt

all:
	/usr/libexec/qemu-kvm  -m 1024 -drive format=raw,file=$(DISK) -nographic
	# /usr/libexec/qemu-kvm  -m 1024 -drive format=raw,file=/dev/nvme0n2 -nographic

mk_boot_disk:
	@if [ ! -e $(DISK) ]; then dd if=/dev/zero of=$(DISK) bs=1M count=100 status=progress; fi

mk_fs: mk_boot_disk
	@losetup $(LOOP) $(DISK)
	@losetup -l $(LOOP)
	@./mkpart.sh $(LOOP)
	@kpartx -av $(LOOP)
	@mkfs.ext4 -q $(LOOP_PART)
	@mount $(LOOP_PART) $(MNT)
	@mkdir $(MNT)/boot
	@grub2-install --boot-directory=$(MNT)/boot/ --force $(LOOP)
	@./mkgrub.sh $(MNT)/boot/grub2/grub.cfg
	@umount $(LOOP_PART)
	@kpartx -dv $(LOOP)
	@losetup -d $(LOOP)
