TOOLPREFIX := riscv64-unknown-elf-
QEMU = qemu-system-riscv64
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

ROOT 	:= $(shell pwd)
SOURCE_ROOT := $(shell pwd)/src
SCRIPT	:= $(ROOT)/script
BUILD_DIR 	:= $(ROOT)/build
U_PROG_DIR	:= $(BUILD_DIR)/user_prog
OBJ_DIR 	:= $(BUILD_DIR)/objs
K := $(ROOT)/src/kernel
U := $(ROOT)/src/user
P := $(ROOT)/src/platform

# CPU数目
ifndef CPUS
CPUS := 2
endif

ifneq ("$(shell if [ -d $(U_PROG_DIR) ]; then echo ok; else echo no; fi)", "ok")
  $(shell mkdir -p $(U_PROG_DIR))
endif

platform ?= qemu

CFLAGS := -I$(P)/$(platform)/include
include $(SCRIPT)/cflags.mk

LDFLAGS := -z max-page-size=4096


export 	platform \
		CC AS LD OBJCOPY OBJDUMP \
		LDFLAGS CFLAGS \
		K U P \
		ROOT SCRIPT OBJ_DIR SOURCE_ROOT U_PROG_DIR

SUB_DIRS := $(K) $(U) $(P) 

all: $(BUILD_DIR)/kernel

$(SUB_DIRS):ECHO
	@make -C $@

SRC := $(shell find $(SOURCE_ROOT)/kernel $(SOURCE_ROOT)/platform -name "*.c" -or -name "*.S")
# 生成内核程序
$(BUILD_DIR)/kernel: $(SUB_DIRS) $(SRC)
	$(LD) $(LDFLAGS) -T $(SCRIPT)/kernel.ld -o $(BUILD_DIR)/kernel $(shell find $(OBJ_DIR)/kernel $(OBJ_DIR)/platform -name "*.o")
	$(OBJDUMP) -S $(BUILD_DIR)/kernel > $(BUILD_DIR)/kernel.asm
	$(OBJDUMP) -t $(BUILD_DIR)/kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(BUILD_DIR)/kernel.sym


# -include kernel/*.d user/*.d

clean: 
	-rm -rf build
	-rm -rf fs.img
	-rm -rf $(SCRIPT)/mkfs

$(SCRIPT)/mkfs: $(SCRIPT)/mkfs.c include/kernel/fs.h include/kernel/param.h
	gcc -Werror -Wall -Iinclude -o $@ $<

# 磁盘映像制作
U_PROG := $(wildcard $(U_PROG_DIR)/_*)
fs.img: $(SCRIPT)/mkfs README $(U_PROG)
	$(SCRIPT)/mkfs fs.img README $(shell find $(U_PROG_DIR) -name "_*")


QEMUOPTS = -machine virt -bios bootloader/sbi-qemu -kernel $(BUILD_DIR)/kernel -m 130M -smp $(CPUS) -nographic
QEMUOPTS += -drive file=fs.img,if=none,format=raw,id=x0
QEMUOPTS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

qemu: $(BUILD_DIR)/kernel fs.img
	$(QEMU) $(QEMUOPTS)

.gdbinit: .gdbinit.tmpl-riscv
	sed "s/:1234/:$(GDBPORT)/" < $^ > $@

qemu-gdb: $(BUILD_DIR)/kernel .gdbinit fs.img
	@echo "*** Now run 'gdb' in another window." 1>&2
	$(QEMU) $(QEMUOPTS) -S -gdb tcp::1234


ECHO:
	@echo $(SUB_DIRS)




# K_DIR 		:= $(shell find $(K) $(P)/$(platform) -type d)
# K_SRC_C 	:= $(foreach dir, $(K_DIR), $(wildcard $(dir)/*.c))
# K_SRC_ASM 	:= $(foreach dir, $(K_DIR), $(wildcard $(dir)/*.S))
# K_OBJ_C 		:= $(patsubst %.c, $(BUILD_OBJ)/%.o, $(notdir $(K_SRC_C)))
# K_OBJ_ASM 		:= $(patsubst %.S, $(BUILD_OBJ)/%.o, $(notdir $(K_SRC_ASM)))

# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
# .PRECIOUS: %.o

# # try to generate a unique GDB port
# GDBPORT = $(shell expr `id -u` % 5000 + 25000)
# # QEMU's gdb stub command line changed in 0.11
# QEMUGDB = $(shell if $(QEMU) -help | grep -q '^-gdb'; \
# 	then echo "-gdb tcp::$(GDBPORT)"; \
# 	else echo "-s -p $(GDBPORT)"; fi)
