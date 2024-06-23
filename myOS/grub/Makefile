DISK = ../disk_grub.img
LOOP = /dev/loop520
LOOP_PART= /dev/mapper/loop520p1
MNT = /mnt

all: install_os
	/usr/libexec/qemu-kvm -m 1024 -drive format=raw,file=$(DISK) -nographic

# 制作硬盘文件
mk_disk:
	@if [ ! -e $(DISK) ]; then \
		dd if=/dev/zero of=$(DISK) bs=1M count=10 status=progress;\
	fi

# 硬盘文件分区
mk_part: mk_disk
	@info=`parted -m $(DISK) print`;				\
	line=`echo "$$info" | wc | awk '{print $$1}'`;			\
	if [ $$line -ge 3 ]; then					\
		echo "disk already parted, skip ..."; 			\
	else 								\
		losetup $(LOOP) $(DISK);				\
		losetup -l $(LOOP);					\
		./mkpart.sh $(LOOP);					\
		losetup -d $(LOOP);					\
	fi

# 安装文件系统及 grub
mk_fs: mk_part
	@fs=`parted -m $(DISK) print | awk -F ":" 'NR==3{print $$5}'`;	\
	if [ "$$fs" != "" ]; then					\
		echo "disk already make filesystem <$$fs>, skip ...";	\
	else								\
		losetup $(LOOP) $(DISK);				\
		kpartx -av $(LOOP);					\
		mkfs.ext4 -q $(LOOP_PART);				\
		mount $(LOOP_PART) $(MNT);				\
		mkdir $(MNT)/boot;					\
		grub2-install --boot-directory=$(MNT)/boot/ --force $(LOOP);\
		./mkgrub.sh $(MNT)/boot/grub2/grub.cfg;			\
		umount $(LOOP_PART);					\
		kpartx -dv $(LOOP);					\
		losetup -d $(LOOP);					\
	fi

# 安装内核
install_os: mk_fs
	@if [ ! -e $(KERNEL) ]; then 					\
		echo "$(KERNEL) not exist, error!!!"; exit -1;		\
	fi
	@if [ "$(KERNEL)" == "" ]; then					\
		echo "'KERNEL' is empty, error!!!"; exit -1;		\
	fi
	@losetup $(LOOP) $(DISK);
	@kpartx -av $(LOOP);
	@mount $(LOOP_PART) $(MNT);
	@cp $(KERNEL) $(MNT)
	@cat $(MNT)/boot/grub2/grub.cfg;
	@ls -al $(MNT);
	@umount $(LOOP_PART);
	@kpartx -dv $(LOOP);
	@losetup -d $(LOOP);
