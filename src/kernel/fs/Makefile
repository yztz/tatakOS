# SRC := $(wildcard *.c)
# BUILD_DIR := $(subst $(SOURCE_ROOT),$(OBJ_DIR),$(shell pwd))
# TARGET := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC))

# all: $(BUILD_DIR) $(TARGET)

# $(BUILD_DIR):
# 	mkdir -p $@

# $(BUILD_DIR)/%.o : %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

SRC := $(wildcard *.c)
BUILD_DIR := $(subst $(SOURCE_ROOT),$(OBJ_DIR),$(shell pwd))
TARGET := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC))
SUB_DIRS := $(call f_subdir)

CFLAGS += -I$(ROOT)/include/kernel
export CFLAGS

all: $(SUB_DIRS) $(BUILD_DIR) $(TARGET)

$(SUB_DIRS):ECHO
	@make -C $@

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@


ECHO:
	@echo $(SUB_DIRS)