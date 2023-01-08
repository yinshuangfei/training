#
# Makefile for Linux samplefs
#
CONFIG_MODULE_SIG=n

obj-m += ysffs.o
ysffs-objs := super.o inode.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd)

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) clean
