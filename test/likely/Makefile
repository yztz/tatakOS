all: likely.S unlikely.S

likely.S: likely.c
	riscv64-unknown-elf-gcc -c -g likely.c -o likely.o
	riscv64-unknown-elf-objdump -S likely.o > likely.S
unlikely.S: unlikely.c
	riscv64-unknown-elf-gcc -c -g unlikely.c -o unlikely.o
	riscv64-unknown-elf-objdump -S unlikely.o > unlikely.S