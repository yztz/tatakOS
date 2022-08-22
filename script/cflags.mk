# 这里可以加些警告过滤器
CFLAGS += -Wno-unused-function

# -fno-omit-frame-pointer:不优化栈帧指针  -O=-O1
CFLAGS += -Wall -Werror -O0 -fno-omit-frame-pointer -ggdb
# medany=PC±2G medlow=+2G&-2G
CFLAGS += -mcmodel=medany -march=rv64g
# 独立程序，不会额外链接其他标准库
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -fstrict-volatile-bitfields
# 关闭栈保护措施 主要是为了防止栈溢出攻击，比如金丝雀之类的机制
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

# Disable PIE when possible (for Ubuntu 16.10 toolchain)
# PIE似乎是一种安全机制
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif