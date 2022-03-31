# 用户标准库.o
SRC 		:= $(wildcard *.c)
BUILD_DIR 	:= $(subst $(SOURCE_ROOT),$(OBJ_DIR),$(shell pwd))
TARGET 		:= $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC)) $(BUILD_DIR)/initcode
PROG 		:= $(patsubst %.c, $(U_PROG_DIR)/_%, $(SRC))

CFLAGS 	+= -I$(U)/include
CFLAGS += -I$(ROOT)/include
export CFLAGS

ULIB_OBJ:= $(BUILD_DIR)/lib/*.o
# lib要最先编译
SUB_DIRS := lib $(shell ls -l | grep ^d | awk '{if($$9 != "lib" && $$9 != "include") print $$9}')

all: $(SUB_DIRS) $(TARGET) $(PROG)

$(SUB_DIRS):ECHO
	@make -C $@

$(U_PROG_DIR)/_% : $(BUILD_DIR)/%.o
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $< $(ULIB_OBJ)

$(BUILD_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
# $(OBJDUMP) -S $@ > $*.asm
# $(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym

# $U/_forktest: $U/forktest.o $(ULIB)
# 	# forktest has less library code linked in - needs to be small
# 	# in order to be able to max out the proc table.
# 	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $U/_forktest $U/forktest.o $U/ulib.o $U/usys.o
# 	$(OBJDUMP) -S $U/_forktest > $U/forktest.asm

# 生成initcode程序
$(BUILD_DIR)/initcode: initcode.S
	# rv64g=rv64imafd（i=integer m=mul/div a=atom f=float d=double）
	@$(CC) $(CFLAGS) -march=rv64g -nostdinc -I. -Ikernel -c initcode.S -o $(BUILD_DIR)/initcode.o
	# -N=Do not page align data, do not make text readonly
	@$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $(BUILD_DIR)/initcode.out $(BUILD_DIR)/initcode.o
	@$(OBJCOPY) -S -O binary $(BUILD_DIR)/initcode.out $(BUILD_DIR)/initcode
	@$(OBJDUMP) -S $(BUILD_DIR)/initcode.o > $(BUILD_DIR)/initcode.asm

ECHO:
	@echo FOUND $(SUB_DIRS)