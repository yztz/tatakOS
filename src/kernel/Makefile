obj-y += console.o
obj-y += exec.o
obj-y += proc.o
obj-y += syscall.o
obj-y += sysfile.o
obj-y += sysproc.o
obj-y += trap.o
obj-y += sys.o
obj-y += pagefault.o

obj-y += asm/


# initcode
kernel_include_dir := $K/include/generated
initcode_bin := $(kernel_include_dir)/initcode_bin.h
$(initcode_bin): initcode.c $(ROOT)/entry/syscall.tbl
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $(BUILD_DIR)/initcode.o
	@$(LD) $(LDFLAGS) -T $(SCRIPT)/initcode.ld -o $(BUILD_DIR)/initcode.out $(BUILD_DIR)/initcode.o
	@$(OBJCOPY) -S -O binary $(BUILD_DIR)/initcode.out $(BUILD_DIR)/initcode
	@$(OBJDUMP) -S $(BUILD_DIR)/initcode.o > $(BUILD_ROOT)/initcode.asm
	@mkdir -p $(kernel_include_dir)
	@$(SCRIPT)/bin2array.py $(BUILD_DIR)/initcode -l 7 -O $@
	@echo -e "GEN\t\tinit_code"

proc.c : $(initcode_bin)
	@touch proc.c