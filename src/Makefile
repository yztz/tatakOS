SHELL = /bin/bash
CFLAGS += -I$(ROOT)/include
# CFLAGS += -I$(ROOT)/include/kernel # deprecated
CFLAGS += -I$K/include

TARGET := $(BUILD_ROOT)/kernel

obj-y+=atomic/
obj-y+=driver/
obj-y+=fs/
obj-y+=kernel/
obj-y+=misc/
obj-y+=mm/
obj-y+=net/
obj-y+=platform/
obj-y+=lib/
obj-y+=tests/

obj-y += device.o
obj-y += ioctl.o
obj-y += main.o
obj-y += panic.o
obj-y += printf.o
obj-y += riscv.o
obj-y += string.o
obj-y += utils.o
obj-y += futex.o


#================GEN kernel=================#
BUILD_DIR := $(subst $K,$(OBJ_DIR),$(shell pwd))

all: start_recursive_build  $(TARGET)

.PHONY: start_recursive_build

start_recursive_build:
	@echo -e "\n\e[37;1mCFLAGS = $(CFLAGS)\e[0m\n"	
	@make -C ./ -f $(SCRIPT)/Makefile.build

# 生成内核程序
$(TARGET): $(BUILD_DIR)/built-in.o
	@echo -e "LD\t\tkernel"
	@$(LD) $(LDFLAGS) -L$(SCRIPT) -T $(SCRIPT)/kernel-$(platform).ld -o $(TARGET) $<
	@$(OBJDUMP) -S $(TARGET) > $(TARGET).asm
	@$(OBJDUMP) -t $(TARGET) | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(TARGET).sym
