MODEL_NAME := user_pmu
SRC_LIST := main.c
KERNELDIR := /home/dev/xilinx-kernel
PWD := $(shell pwd) 

ifneq ($(KERNELRELEASE),)
    $(MODEL_NAME)-objs := $(patsubst %.c,%.o,$(SRC_LIST))
    obj-m := $(MODEL_NAME).o
else

modules: 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules 

modules_install: 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install 
clean:
	@-rm -rf *.o *.ko *.symvers *.order *.mod.c .*.cmd .tmp_versions
endif
