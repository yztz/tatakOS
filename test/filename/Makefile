TEST_NAME := filename

all: $(TEST_NAME)
	./$(TEST_NAME)

gdb: $(TEST_NAME)
	gdb $(TEST_NAME) --tui

$(TEST_NAME): $(TEST_NAME).c
	gcc -O0 -ggdb $< -o $@