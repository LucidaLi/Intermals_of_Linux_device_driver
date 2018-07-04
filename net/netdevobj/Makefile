#a simple kernel module to display all the net devices in the system
MOD_NAME := netdevobj
obj-m := $(MOD_NAME).o
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE)  -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -f *.o *.ko *.mod.* *.order *.symvers
