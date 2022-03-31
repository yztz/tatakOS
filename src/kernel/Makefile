EXTRA_CFLAGS := -I$(ROOT)/include/kernel

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

start_recursive_build:
	@make -C ./ -f $(SCRIPT)/Makefile.build

# 生成内核程序
$(TARGET): $(BUILD_DIR)/built-in.o
	@$(LD) $(LDFLAGS) -T $(SCRIPT)/kernel.ld -o $(TARGET) $<
	@$(OBJDUMP) -S $(TARGET) > $(TARGET).asm
	@$(OBJDUMP) -t $(TARGET) | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(TARGET).sym


.PHONY: start_recursive_build