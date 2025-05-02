obj-m += kmod.o
CURRENT_PATH := $(shell pwd)
LINUX_KERNEL := $(shell uname -r)
LINUX_KERNEL_PATH := /lib/modules/$(LINUX_KERNEL)/build

all:
	@echo "==> CURRENT_PATH = $(CURRENT_PATH)"
	@echo "==> KERNEL_VERSION = $(LINUX_KERNEL)"
	@echo "==> LINUX_KERNEL_PATH = $(LINUX_KERNEL_PATH)"
	@echo "Building in $(CURRENT_PATH)"
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean

