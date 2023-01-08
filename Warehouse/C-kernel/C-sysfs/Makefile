obj-m:= dc_gpio.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD :=$(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules -j4

PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean -j4



PHONY: in
in: all
	insmod ./dc_gpio.ko

PHONY: rm
rm:
	rmmod dc_gpio
