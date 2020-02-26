install: boot.asm kernel.cpp
	nasm -felf32 boot.asm -o boot.o && i686-elf-g++ -c kernel.cpp -o kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti \
	&& i686-elf-gcc -T linker.ld -o kernel.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

clean:
	rm boot.o kernel.o kernel.bin

run:
	qemu-system-i386 -kernel kernel.bin
