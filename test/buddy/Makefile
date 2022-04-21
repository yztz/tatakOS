all: buddy
	./buddy

gdb: buddy
	gdb buddy --tui

buddy: main.c
	gcc -O0 -ggdb main.c -o buddy
	