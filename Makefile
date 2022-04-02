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

# 注意：若修改了系统调用，（虽然已有依赖支持，但保险起见）请重新编译：make clean && make 
syscall_hdr := include/kernel/syscall.h

kernel: $(syscall_hdr)
	@make -C src/kernel
	@echo -e "\n\033[32;1mKERNEL BUILD SUCCESSFUL!\033[0m\n"

user: $(syscall_hdr)
	@if [ ! -d $(U_PROG_DIR) ]; then mkdir -p $(U_PROG_DIR); fi
	@make -C src/user
	@echo -e "\n\033[32;1mUSER EXE BUILD SUCCESSFUL!\033[0m\n"

$(syscall_hdr): entry/syscall.tbl
	@$(SCRIPT)/sys_tbl.py entry/syscall.tbl -o $@
	@$(SCRIPT)/sys_tbl.py entry/syscall.tbl -o $K/include/syscall_tbl.h -t tbl

qemu: $(TARGET) fs.img
	$(QEMU) $(QEMUOPTS)

qemu-gdb: $(TARGET) .gdbinit fs.img
	@echo "*** Now run 'gdb' in another window." 1>&2
	$(QEMU) $(QEMUOPTS) -S -gdb tcp::1234

clean: 
	-@rm -rf build
	-@rm -rf fs.img
	-@rm -rf $(SCRIPT)/mkfs
	-@rm -rf $K/__bin__initcode
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