#a simple kmod to test the kvm
obj-m := vmx_kmod.o
vmx_kmod-objs := vmx_test.o ext_func.o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE)  -C $(KERNELDIR)  M=$(PWD) modules
clean:
	rm -f *.o *.ko *.mod.* *.order *.symvers
