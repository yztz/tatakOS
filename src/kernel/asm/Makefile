SRC := $(wildcard *.S)
BUILD_DIR := $(subst $(SOURCE_ROOT),$(OBJ_DIR),$(shell pwd))
TARGET := $(patsubst %.S, $(BUILD_DIR)/%.o, $(SRC))

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o : %.S
	$(CC) $(CFLAGS) -c $< -o $@
