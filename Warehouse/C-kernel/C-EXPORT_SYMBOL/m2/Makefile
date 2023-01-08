NAME=m2
obj-m:= $(NAME).o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD :=$(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

in: rm
	lsmod | grep $(NAME) | grep -v grep 1>/dev/null; if [ $$? == 1 ]; then insmod $(NAME).ko ; fi
rm:
	lsmod | grep $(NAME) | grep -v grep 1>/dev/null; if [ $$? == 0 ]; then rmmod $(NAME); fi
.PHONY:clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

