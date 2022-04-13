SHELL = /bin/bash
MAKEFLAGS += --no-print-directory

#===========================CONFIG=================================#
# CPU NUMS(qemu)
CPUS ?= 2
# platform [qemu|k210]
platform ?= k210
# debug [on|off]
debug ?= on
# serial-port
serial-port := /dev/ttyUSB0
# gdb-port
gdb_port := 1234
# architecture
arch := riscv64

#==========================DIR INFO================================#
ROOT 	:= $(shell pwd)
SOURCE_ROOT := $(shell pwd)/src
SCRIPT	:= $(ROOT)/script
TOOL    := $(ROOT)/tools
BUILD_ROOT 	:= $(ROOT)/build
U_PROG_DIR	:= $(BUILD_ROOT)/user_prog
OBJ_DIR 	:= $(BUILD_ROOT)/objs
K := $(ROOT)/src/kernel
U := $(ROOT)/src/user
P := $(ROOT)/src/kernel/platform
A := $(ROOT)/src/kernel/arch

#==========================TOOLCHAINS==============================#
TOOLPREFIX := riscv64-unknown-elf-
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

#=============================EXPORT===============================#

export ROOT SCRIPT OBJ_DIR SOURCE_ROOT U_PROG_DIR K U P A BUILD_ROOT TOOL
export CC AS LD OBJCOPY OBJDUMP
export debug platform arch

#=============================FLAGS=+==============================#
# platform
CFLAGS += -I$(P)/$(platform)/include
ifeq ("${platform}", "qemu")
  CFLAGS += -DQEMU
  CFLAGS += -DCPUS=$(CPUS)
else
  CFLAGS += -DK210
endif

# debug
ifeq ("${debug}", "on")
  CFLAGS += -DDEBUG
endif

include $(SCRIPT)/cflags.mk
LDFLAGS := -z max-page-size=4096

export LDFLAGS CFLAGS

#============================QEMU==================================#
QEMU = qemu-system-riscv64
QEMUOPTS += -machine virt -bios bootloader/sbi-qemu -kernel $(BUILD_ROOT)/kernel -m 130M -smp $(CPUS) -nographic
QEMUOPTS += -drive file=fs.img,if=none,format=raw,id=x0
QEMUOPTS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

#===========================RULES BEGIN============================#
all: kernel

run: kernel
ifeq ("$(platform)", "k210") # k210
	$(OBJCOPY) $(BUILD_ROOT)/kernel -S -O binary $(BUILD_ROOT)/kernel.bin
	$(OBJCOPY) bootloader/sbi-k210 -S -O binary $(BUILD_ROOT)/k210.bin
	dd if=$(BUILD_ROOT)/kernel.bin of=$(BUILD_ROOT)/k210.bin bs=128k seek=1
	sudo chmod 777 $(serial-port)
	$(TOOL)/kflash.py -p $(serial-port) -b 1500000 -B dan $(BUILD_ROOT)/k210.bin
	$(TOOL)/read_serial.py
else ifeq ("$(platform)", "qemu") # qemu
	$(QEMU) $(QEMUOPTS) $(EXTRA_QEMUOPTS)
else # others
	@echo -e "\n\033[31;1mUNSUPPORT PLATFORM!\033[0m\n"
endif

ifeq ("$(platform)", "qemu")
run: fs.img
endif


syscall := include/kernel/syscall_gen.h

kernel: $(syscall)
	@make -C src/kernel
	@echo -e "\n\033[32;1mKERNEL BUILD SUCCESSFUL!\033[0m\n"

user: $(syscall)
	@mkdir -p $(U_PROG_DIR)
	@make -C src/user
	@echo -e "\n\033[32;1mUSER EXE BUILD SUCCESSFUL!\033[0m\n"

$(syscall): entry/syscall.tbl
	@echo -e "GEN\t\tsyscall"
	@$(SCRIPT)/sys_tbl.py entry/syscall.tbl -o include/kernel/syscall_gen.h -t tbl
	@$(SCRIPT)/sys_tbl.py entry/syscall.tbl -o include/kernel/syscall.h -t hdr

SBI_TARGET_PATH := target/riscv64imac-unknown-none-elf/debug
sbi-k210:
	@cd bootloader/rustsbi-k210 && cargo make
	cp bootloader/rustsbi-k210/$(SBI_TARGET_PATH)/rustsbi-k210 bootloader/$@
sbi-qemu:
	@cd bootloader/rustsbi-qemu && cargo make
	cp bootloader/rustsbi-qemu/$(SBI_TARGET_PATH)/rustsbi-qemu bootloader/$@

clean: 
	-@rm -rf build
	-@rm -rf fs.img
	-@rm -rf $(SCRIPT)/mkfs
	-@rm -rf include/kernel/syscall_gen.h
	-@rm -rf include/kernel/syscall.h
	-@rm -rf $K/include/initcode_bin.h
	@echo -e "\n\033[32;1mCLEAN DONE\033[0m\n"

$(SCRIPT)/mkfs: $(SCRIPT)/mkfs.c include/kernel/fs.h include/kernel/param.h
	gcc -Werror -Wall -Iinclude -o $@ $<

# 磁盘映像制作
fs.img: user $(SCRIPT)/mkfs
	@$(SCRIPT)/mkfs fs.img README $(shell find $(U_PROG_DIR) -name "_*")


.gdbinit: .gdbinit.tmpl-riscv
	sed "s/:1234/:$(GDBPORT)/" < $^ > $@

.PHONY: fs.img qemu clean all user kernel entry sbi-k210

#===========================RULES END==============================#