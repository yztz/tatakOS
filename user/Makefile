ARCH ?= riscv64
cmake_build_args := -DARCH=$(ARCH)
cmake_build_args += -DCMAKE_CXX_COMPILER_WORKS=1 -DCMAKE_C_COMPILER_WORKS=1
# riscv64-unknown-elf-
OBJCOPY := $(ARCH)-unknown-elf-objcopy
OBJDUMP := $(ARCH)-unknown-elf-objdump

ifdef I
  CMAKE := cmake > /dev/null
  MAKE := make > /dev/null
else
  CMAKE := cmake
  MAKE := make
endif


makefile := $(U_OBJ_DIR)/Makefile

binary: makefile
	@cd $(U_OBJ_DIR) && $(MAKE)
	@cp -r $(U_OBJ_DIR)/riscv64/* $(U_PROG_DIR)

makefile:
	@mkdir -p $(U_OBJ_DIR)
	@cd $(U_OBJ_DIR) && $(CMAKE) $(cmake_build_args) $(shell pwd)

all: binary

clean:
	@rm -rf build target asm riscv64
