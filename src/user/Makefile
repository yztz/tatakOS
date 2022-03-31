CFLAGS += -I$(U)/include
CFLAGS += -I$(ROOT)/include
export CFLAGS

BUILD_DIR := $(subst $(SOURCE_ROOT),$(OBJ_DIR),$(shell pwd))

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

# 目标可执行程序
target_prog := $(foreach p, $(prog), $(U_PROG_DIR)/$(p))
# 程序构建路径
src_dir := $(patsubst _%, $(shell pwd)/%, $(prog))
# 库目标
ulib_obj := $(BUILD_DIR)/lib/built-in.o

all: $(ulib_obj) $(src_dir) $(target_prog)

$(target_prog) : $(U_PROG_DIR)/_% : $(BUILD_DIR)/%/built-in.o
	@$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $< $(ulib_obj)

.PHONY: $(src_dir)
$(src_dir):
	@make -C $@ -f $(SCRIPT)/Makefile.build

$(ulib_obj) : ulib

.PHONY: ulib
ulib: 
	@make -C lib -f $(SCRIPT)/Makefile.build

# $U/_forktest: $U/forktest.o $(ULIB)
# 	# forktest has less library code linked in - needs to be small
# 	# in order to be able to max out the proc table.
# 	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $U/_forktest $U/forktest.o $U/ulib.o $U/usys.o
# 	$(OBJDUMP) -S $U/_forktest > $U/forktest.asm

# 生成initcode程序
# $(BUILD_DIR)/initcode: initcode.S
# # rv64g=rv64imafd（i=integer m=mul/div a=atom f=float d=double）
# 	@$(CC) $(CFLAGS) -march=rv64g -nostdinc -I. -Ikernel -c initcode.S -o $(BUILD_DIR)/initcode.o
# # -N=Do not page align data, do not make text readonly
# 	@$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $(BUILD_DIR)/initcode.out $(BUILD_DIR)/initcode.o
# 	@$(OBJCOPY) -S -O binary $(BUILD_DIR)/initcode.out $(BUILD_DIR)/initcode
# 	@$(OBJDUMP) -S $(BUILD_DIR)/initcode.o > $(BUILD_DIR)/initcode.asm

