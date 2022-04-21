SHELL := /bin/bash
CFLAGS += -I$U/include
CFLAGS += -I$(ROOT)/include

BUILD_DIR := $(subst $U,$(U_OBJ_DIR),$(shell pwd))

prog += _cat
prog += _echo
prog += _forktest
prog += _grep
prog += _grind
prog += _init
prog += _kill
prog += _ln
prog += _ls
prog += _mkdir
prog += _rm
prog += _sh
prog += _stressfs
prog += _usertests
prog += _wc
prog += _zombie
prog += _ctxswitch
prog += _freemem

# 带有路径的目标可执行程序
target_prog := $(foreach p, $(prog), $(U_PROG_DIR)/$(p))
# 程序构建路径
src_dir := $(patsubst _%, $(shell pwd)/%, $(prog))
# 库目标
ulib_obj := $(BUILD_DIR)/lib/built-in.o

all: $(ulib_obj) $(src_dir) $(target_prog)

$(target_prog) : $(U_PROG_DIR)/_% : $(BUILD_DIR)/%/built-in.o $(ulib_obj)
	@$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^
	@echo -e "LD\t\t$(notdir $@)"

.PHONY: $(src_dir)
$(src_dir):
	@make -C $@ -f $(SCRIPT)/Makefile.build

$(ulib_obj) : 
	@make -C lib -f $(SCRIPT)/Makefile.build

# .PHONY: initcode
# initcode:
# 	@$(CC) $(CFLAGS) -march=rv64g -nostdinc -I. -Ikernel -c initcode.S -o $(BUILD_DIR)/initcode.o
# 	@$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $(BUILD_DIR)/initcode.out $(BUILD_DIR)/initcode.o
# 	@$(OBJCOPY) -S -O binary $(BUILD_DIR)/initcode.out $(BUILD_DIR)/initcode
# 	@$(OBJDUMP) -S $(BUILD_DIR)/initcode.o > $(BUILD_DIR)/initcode.asm

# $U/_forktest: $U/forktest.o $(ULIB)
# 	# forktest has less library code linked in - needs to be small
# 	# in order to be able to max out the proc table.
# 	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $U/_forktest $U/forktest.o $U/ulib.o $U/usys.o
# 	$(OBJDUMP) -S $U/_forktest > $U/forktest.asm

