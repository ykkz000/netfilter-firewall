export TARGET			:= netfilter-firewall
export K_DIR			:= /lib/modules/$(shell uname -r)/build
export PROJECT_DIR		:= $(shell pwd)
export SRC_DIR			:= $(PROJECT_DIR)/src
export INCLUDE_DIR		:= $(PROJECT_DIR)/include
export BUILD_DIR		:= $(PROJECT_DIR)/build
export TARGET_DIR		:= $(PROJECT_DIR)/target

ifneq ($(KERNELRELEASE),)
obj-m       := ${TARGET}.o
${TARGET}-y := main.o fw.o uac.o
ccflags-y   := -I$(INCLUDE_DIR)
else

.PHONY:build run clean

build: $(BUILD_DIR)/Makefile
	make -C $(K_DIR) M=$(BUILD_DIR) src=$(SRC_DIR) modules
	cp $(BUILD_DIR)/*.ko $(TARGET_DIR)

clean:
	make -C $(K_DIR) M=$(BUILD_DIR) src=$(SRC_DIR) clean

$(BUILD_DIR)/Makefile: $(BUILD_DIR)
	touch "$@"

endif