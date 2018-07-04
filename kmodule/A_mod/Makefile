#Chap1: a simple demo kernel module
obj-m := demodev.o
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE)  -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -f *.o *.ko *.mod.* *.order *.symvers
