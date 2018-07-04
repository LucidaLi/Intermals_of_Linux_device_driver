obj-m := ramhd_req.o
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE)  -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -f *.o *.ko *.mod.* *.order *.symvers
