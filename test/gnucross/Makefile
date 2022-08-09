TEST_NAME := gnucross

$(TEST_NAME): $(TEST_NAME).c
	riscv64-linux-gnu-gcc --static -O0 -ggdb $< -o $@

all: $(TEST_NAME)
	./$(TEST_NAME)

gdb: $(TEST_NAME)
	gdb $(TEST_NAME) --tui

