SRC := $(wildcard *.c)
BUILD_DIR := $(subst $(SOURCE_ROOT),$(OBJ_DIR),$(shell pwd))
TARGET := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC)) $(BUILD_DIR)/usys.o

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 一般规则
$(BUILD_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# usys规则
usys.S : $(SCRIPT)/usys.pl
	perl $(SCRIPT)/usys.pl > usys.S

$(BUILD_DIR)/usys.o : usys.S
	$(CC) $(CFLAGS) -c -o $@ $<
