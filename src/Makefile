SHELL = /bin/bash
CFLAGS += -I$(ROOT)/include
CFLAGS += -I$K/include

TARGET := $(BUILD_ROOT)/kernel

obj-y+=atomic/
obj-y+=driver/
obj-y+=fs/
obj-y+=kernel/
obj-y+=misc/
obj-y+=mm/
obj-y+=platform/
obj-y+=lib/


obj-y += kprint.o
obj-y += main.o
obj-y += printf.o
obj-y += console.o
obj-y += riscv.o
obj-y += utils.o



#================GEN kernel=================#
BUILD_DIR := $(subst $K,$(OBJ_DIR),$(shell pwd))

include $(SCRIPT)/colors.mk

all: start_recursive_build  $(TARGET)

.PHONY: start_recursive_build

start_recursive_build:
	$(V)make -C ./ -f $(SCRIPT)/Makefile.build

# 生成内核程序
$(TARGET): $(BUILD_DIR)/built-in.o
	$(call make_echo_ld,kernel)
	$(V)$(LD) $(LDFLAGS) -L$(SCRIPT) -T $(SCRIPT)/kernel-$(platform).ld -o $(TARGET) $<
	$(V)$(OBJDUMP) -S $(TARGET) > $(TARGET).asm
	$(V)$(OBJDUMP) -t $(TARGET) | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(TARGET).sym
