ARCH ?= riscv64
MODE ?= release
CHAPTER ?= 7
app_dir = src
src_dir := build/$(ARCH)
bin_dir := build/bin
out_dir := target
cmake_build_args := -DARCH=$(ARCH)
cmake_build_args += -DCMAKE_CXX_COMPILER_WORKS=1 -DCMAKE_C_COMPILER_WORKS=1
ifeq ($(MODE), release)
cmake_build_args += -DCMAKE_BUILD_TYPE=Release
else ifeq ($(MODE), debug)
cmake_build_args += -DCMAKE_BUILD_TYPE=Debug
endif

# riscv64-unknown-elf-
OBJCOPY := $(ARCH)-unknown-elf-objcopy
OBJDUMP := $(ARCH)-unknown-elf-objdump
CP := cp 


makefile := $(U_OBJ_DIR)/Makefile

binary: makefile
	@cd $(U_OBJ_DIR) && make
	@$(CP) -r $(U_OBJ_DIR)/riscv64/* $(U_PROG_DIR)

makefile:
	@mkdir -p $(U_OBJ_DIR)
	@cd $(U_OBJ_DIR) && cmake $(cmake_build_args) $(shell pwd)

all: binary

clean:
	@rm -rf build target asm riscv64
