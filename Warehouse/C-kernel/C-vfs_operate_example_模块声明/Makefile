obj-m:= vfs_operate_example.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD :=$(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules -j4

.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean -j4


.PHONY: in
in: all
	insmod ./vfs_operate_example.ko

.PHONY: rm
rm:
	rmmod vfs_operate_example