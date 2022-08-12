SHELL = /bin/bash
MAKEFLAGS += --no-print-directory

#===========================CONFIG=================================#
# CPU NUMS(qemu)
CPUS ?= 4
# platform [qemu|k210]
platform ?= qemu
# debug [on|off]
debug ?= off
# serial-port
serial-port := /dev/ttyUSB0
# gdb-port
gdb_port := 1234
# architecture
arch := riscv64

display_todo_info ?= off

#==========================DIR INFO================================#
ROOT 	:= $(shell pwd)
SCRIPT	:= $(ROOT)/script
TOOL    := $(ROOT)/tools
BUILD_ROOT 	:= $(ROOT)/build
U_PROG_DIR	:= $(BUILD_ROOT)/user_prog
OBJ_DIR 	:= $(BUILD_ROOT)/objs
U_OBJ_DIR 	:= $(BUILD_ROOT)/u_objs
K := $(ROOT)/src
U := $(ROOT)/user
P := $(ROOT)/src/platform

#==========================TOOLCHAINS==============================#
TOOLPREFIX := riscv64-unknown-elf-
# TOOLPREFIX := /opt/kendryte-toolchain/bin/riscv64-unknown-elf-
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

#=============================EXPORT===============================#

export ROOT SCRIPT OBJ_DIR U_OBJ_DIR U_PROG_DIR K U P BUILD_ROOT TOOL
export CC AS LD OBJCOPY OBJDUMP
export debug platform arch

#=============================FLAGS=+==============================#
# platform
CFLAGS += -I$(P)/$(platform)/include
CFLAGS += $(EXTRA_CFLAGS)
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

ifeq ("${display_todo_info}", "on")
	CFLAGS += -DTODO
endif

include $(SCRIPT)/cflags.mk
LDFLAGS := -z max-page-size=4096

export LDFLAGS CFLAGS

#============================QEMU==================================#
QEMU = qemu-system-riscv64
QEMUOPTS += -machine virt -bios bootloader/sbi-qemu -kernel $(BUILD_ROOT)/kernel -m 128M -smp $(CPUS) -nographic
QEMUOPTS += -drive file=$(fs.img),if=none,format=raw,id=x0
QEMUOPTS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

#===========================RULES BEGIN============================#
# all: kernel
# 	$(OBJCOPY) $(BUILD_ROOT)/kernel -S -O binary $(BUILD_ROOT)/kernel.bin
# 	$(OBJCOPY) bootloader/sbi-k210 -S -O binary $(BUILD_ROOT)/k210.bin
# 	dd if=$(BUILD_ROOT)/kernel.bin of=$(BUILD_ROOT)/k210.bin bs=128k seek=1
# 	mv $(BUILD_ROOT)/k210.bin os.bin
all: kernel
	cp $(BUILD_ROOT)/kernel kernel-qemu
	cp bootloader/sbi-qemu sbi-qemu

run: kernel
ifeq ("$(platform)", "k210") # k210
	$(OBJCOPY) $(BUILD_ROOT)/kernel -S -O binary $(BUILD_ROOT)/kernel.bin
	$(OBJCOPY) bootloader/sbi-k210 -S -O binary $(BUILD_ROOT)/k210.bin
	dd if=$(BUILD_ROOT)/kernel.bin of=$(BUILD_ROOT)/k210.bin bs=128k seek=1
	sudo chmod 777 $(serial-port)
	$(TOOL)/kflash.py -p $(serial-port) -b 1500000 -B dan -t $(BUILD_ROOT)/k210.bin
#	python3 -m serial.tools.miniterm --eol LF --dtr 0 --rts 0 --filter direct $(serial-port) 115200
else ifeq ("$(platform)", "qemu") # qemu
	$(QEMU) $(QEMUOPTS) $(EXTRA_QEMUOPTS)
else # others
	@echo -e "\n\033[31;1mUNSUPPORT PLATFORM!\033[0m\n"
endif

GEN_HEADER_DIR := $(ROOT)/include/generated

syscall := $(GEN_HEADER_DIR)/syscall_gen.h
profile := $(GEN_HEADER_DIR)/profile_gen.h

kernel: $(syscall)
	@make -C $K
	@echo -e "\n\033[32;1mKERNEL BUILD SUCCESSFUL!\033[0m\n"

$(syscall): entry/syscall.tbl
	@echo -e "GEN\t\tsyscall"
	@mkdir -p $(GEN_HEADER_DIR)
	@python3 $(SCRIPT)/sys_tbl.py $< -o $(GEN_HEADER_DIR)/syscall_gen.h -t tbl
	@python3 $(SCRIPT)/sys_tbl.py $< -o $(GEN_HEADER_DIR)/syscall.h -t hdr

ifeq ("${debug}", "on")
kernel: $(profile)

$(profile): entry/profile.tbl
	@echo -e "GEN\t\tprofile"
	@mkdir -p $(GEN_HEADER_DIR)
	@python3 $(SCRIPT)/profile_tbl.py $< -o $@
endif

SBI_TARGET_PATH := target/riscv64imac-unknown-none-elf/debug
sbi-k210:
	@cd bootloader/rustsbi-k210 && cargo make
	cp bootloader/rustsbi-k210/$(SBI_TARGET_PATH)/rustsbi-k210 bootloader/$@
sbi-qemu:
	@cd bootloader/rustsbi-qemu && cargo make
	cp bootloader/rustsbi-qemu/$(SBI_TARGET_PATH)/rustsbi-qemu bootloader/$@

clean: 
	-@rm -rf $(BUILD_ROOT)
	-@rm -rf $(SCRIPT)/mkfs
	-@rm -rf $(GEN_HEADER_DIR)
	-@rm -rf os.bin
	-@rm -rf sbi-qemu
	-@rm -rf kernel-qemu
	-@rm -rf $K/include/generated
	@echo -e "\n\033[32;1mCLEAN DONE\033[0m\n"

fs.img = $(BUILD_ROOT)/fs.img

ifeq ("$(platform)", "qemu")
run: $(fs.img)
endif

# 磁盘映像制作
MNT_DIR := $(BUILD_ROOT)/mnt
fs.img : $(fs.img)

$(MNT_DIR):
	@mkdir -p $(MNT_DIR)

# $(fs.img): user
$(fs.img): user $(MNT_DIR)
	@dd if=/dev/zero of=$@ bs=1M count=256
	@mkfs.vfat -F 32 -s 8 $@
	@sudo mount $@ $(MNT_DIR)
	@sudo cp -r $(U_PROG_DIR)/* $(MNT_DIR)/
	@sudo umount $(MNT_DIR)


# $(SCRIPT)/mkfs: $(SCRIPT)/mkfs.c include/fs/fs.h include/param.h
# 	gcc -Werror -Wall -Iinclude -o $@ $<

user: $(syscall)
	@mkdir -p $(U_PROG_DIR)
	@make -C $U
	@cp -r $U/raw/* $(U_PROG_DIR)
	@echo -e "\n\033[32;1mUSER EXE BUILD SUCCESSFUL!\033[0m\n"

mnt: $(fs.img)
	@sudo mount $< $(MNT_DIR)
umnt: $(MNT_DIR)
	@sudo umount $(MNT_DIR)

sdcard: $(fs.img)
	sudo dd if=$(fs.img) of=/dev/sdb bs=4M

.gdbinit: .gdbinit.tmpl-riscv
	sed "s/:1234/:$(GDBPORT)/" < $^ > $@

.PHONY: qemu clean all user kernel entry sbi-k210 fs.img mnt sdcard

#===========================RULES END==============================#
