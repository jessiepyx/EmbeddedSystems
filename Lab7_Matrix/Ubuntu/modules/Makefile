obj-m += driver.o

PWD := $(shell pwd)
ARGS := ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
KERNEL_DIR := $(PWD)/../../../Lab6_Kernel/Ubuntu/linux-4.9/

all:
	make $(ARGS) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
clean:
	rm *.o *.ko *.mod.c

.PHONY: clean
