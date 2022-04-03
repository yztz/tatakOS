export SHELL = /bin/bash
MAKEFLAGS += --no-print-directory
export MAKEFLAGS

#==========================TOOLCHAINS==============================#
TOOLPREFIX := riscv64-unknown-elf-
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

export CC AS LD OBJCOPY OBJDUMP

#============================QEMU==================================#
QEMU = qemu-system-riscv64
QEMUOPTS = -machine virt -bios bootloader/sbi-qemu -kernel $(TARGET) -m 130M -smp $(CPUS) -nographic
QEMUOPTS += -drive file=fs.img,if=none,format=raw,id=x0
QEMUOPTS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

#==========================DIR INFO================================#
ROOT 	:= $(shell pwd)
SOURCE_ROOT := $(shell pwd)/src
SCRIPT	:= $(ROOT)/script
BUILD_ROOT 	:= $(ROOT)/build
U_PROG_DIR	:= $(BUILD_ROOT)/user_prog
OBJ_DIR 	:= $(BUILD_ROOT)/objs
K := $(ROOT)/src/kernel
U := $(ROOT)/src/user
P := $(ROOT)/src/kernel/platform

export ROOT SCRIPT OBJ_DIR SOURCE_ROOT U_PROG_DIR K U P BUILD_ROOT

#============================TARGET================================#
TARGET := $(BUILD_ROOT)/kernel
export TARGET

#===========================CPU NUM================================#
ifndef CPUS
CPUS := 2
endif

#======================PLATFORM [qemu|k210]========================#
platform ?= qemu
ifeq ("${platform}", "qemu")
  CFLAGS += -DQEMU
else
  CFLAGS += -DK210
endif

export platform

#=========================DEBUG [on|off]===========================#
debug ?= on
ifeq ("${debug}", "on")
  CFLAGS += -DDEBUG
endif

export debug

#=============================CFLAGS===============================#
CFLAGS += -I$(P)/$(platform)/include
include $(SCRIPT)/cflags.mk
LDFLAGS := -z max-page-size=4096

export LDFLAGS CFLAGS


#===========================RULES BEGIN============================#
all: kernel

syscall := include/kernel/syscall_gen.h

kernel: $(syscall)
	@make -C src/kernel
	@echo -e "\n\033[32;1mKERNEL BUILD SUCCESSFUL!\033[0m\n"

user: $(syscall)
	@if [ ! -d $(U_PROG_DIR) ]; then mkdir -p $(U_PROG_DIR); fi
	@make -C src/user
	@echo -e "\n\033[32;1mUSER EXE BUILD SUCCESSFUL!\033[0m\n"

$(syscall): entry/syscall.tbl
	@echo -e "GEN\t\tsyscall"
	@$(SCRIPT)/sys_tbl.py entry/syscall.tbl -o include/kernel/syscall_gen.h -t tbl
	@$(SCRIPT)/sys_tbl.py entry/syscall.tbl -o include/kernel/syscall.h -t hdr

qemu: $(TARGET) fs.img
	$(QEMU) $(QEMUOPTS)

qemu-gdb: $(TARGET) .gdbinit fs.img
	@echo "*** Now run 'gdb' in another window." 1>&2
	$(QEMU) $(QEMUOPTS) -S -gdb tcp::1234

clean: 
	-@rm -rf build
	-@rm -rf fs.img
	-@rm -rf $(SCRIPT)/mkfs
	-@rm -rf include/kernel/syscall_gen.h
	-@rm -rf include/kernel/syscall.h
	@echo -e "\n\033[32;1mCLEAN DONE\033[0m\n"

$(SCRIPT)/mkfs: $(SCRIPT)/mkfs.c include/kernel/fs.h include/kernel/param.h
	gcc -Werror -Wall -Iinclude -o $@ $<

# 磁盘映像制作
fs.img: user $(SCRIPT)/mkfs
	@$(SCRIPT)/mkfs fs.img README $(shell find $(U_PROG_DIR) -name "_*")


$(TARGET): kernel

.gdbinit: .gdbinit.tmpl-riscv
	sed "s/:1234/:$(GDBPORT)/" < $^ > $@

.PHONY: fs.img qemu clean all user kernel entry

#===========================RULES END==============================#