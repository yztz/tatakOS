SHELL := /bin/bash

SRC := $(wildcard *.S)
BUILD_DIR := $(subst $(SOURCE_ROOT),$(OBJ_DIR),$(shell pwd))
TARGET := $(patsubst %.S, $(BUILD_DIR)/%.o, $(SRC))
SUB_DIRS := $(filter-out $(EXCLUDE_DIR),$(shell ls -l | grep ^d | awk '{ print $$9 }'))

all:$(SUB_DIRS) $(BUILD_DIR) $(TARGET)

$(SUB_DIRS):ECHO
	@make -C $@

$(BUILD_DIR):
	@mkdir -p $@
	@echo "MAKE DIR"

$(BUILD_DIR)/%.o : %.S
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo -e "CC\t$<"
