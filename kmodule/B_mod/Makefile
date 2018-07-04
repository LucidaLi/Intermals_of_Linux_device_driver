#Chap1: a simple demo kernel module
obj-m := dep_on_A.o
KERNELDIR := /lib/modules/$(shell uname -r)/build
KBUILD_EXTMOD := /home/dennis/workspace/Linux/book/kmodule/A_mod
PWD := $(shell pwd)

default:
	$(MAKE)  -C $(KERNELDIR)  M=$(PWD) modules
clean:
	rm -f *.o *.ko *.mod.* *.order *.symvers
