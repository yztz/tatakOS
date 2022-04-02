export SHELL := /bin/bash
EXTRA_CFLAGS := -I$(ROOT)/include/kernel
EXTRA_CFLAGS += -I$K/include

obj-y += console.o
obj-y += exec.o
obj-y += main.o
obj-y += plic.o
obj-y += proc.o
obj-y += sleeplock.o
obj-y += spinlock.o
obj-y += syscall.o
obj-y += sysfile.o
obj-y += sysproc.o
obj-y += trap.o

obj-y += platform/
obj-y += asm/
obj-y += fs/
obj-y += lib/
obj-y += mm/

#================GEN kernel=================#
BUILD_DIR := $(subst $(SOURCE_ROOT),$(OBJ_DIR),$(shell pwd))

all: start_recursive_build  $(TARGET)

.PHONY: start_recursive_build

start_recursive_build:
	@make -C ./ -f $(SCRIPT)/Makefile.build

# 生成内核程序
$(TARGET): $(BUILD_DIR)/built-in.o
	@echo -e "LD\t\tkernel"
	@$(LD) $(LDFLAGS) -T $(SCRIPT)/kernel.ld -o $(TARGET) $<
	@$(OBJDUMP) -S $(TARGET) > $(TARGET).asm
	@$(OBJDUMP) -t $(TARGET) | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(TARGET).sym

# initcode
initcode_bin := include/initcode_bin.h
$(initcode_bin): initcode.c $(ROOT)/entry/syscall.tbl
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $(BUILD_DIR)/initcode.o
	@$(LD) $(LDFLAGS) -N -e entry -Ttext 0 -o $(BUILD_DIR)/initcode.out $(BUILD_DIR)/initcode.o
	@$(OBJCOPY) -S -O binary $(BUILD_DIR)/initcode.out $(BUILD_DIR)/initcode
	@$(OBJDUMP) -S $(BUILD_DIR)/initcode.o > $(BUILD_ROOT)/initcode.asm
	@$(SCRIPT)/bin2array.py $(BUILD_DIR)/initcode -l 7 -O $@
	@echo -e "GEN\t\tinit_code"

proc.c : $(initcode_bin)
	@touch proc.c