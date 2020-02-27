FONT_NAME = 'Goha-16'
FONT_EXT = 'psfu.gz'
BIN_PATH = '../bin'
ISO_NAME = 'kernel.iso'

install: compile iso

compile: boot.asm kernel.cpp font
	@nasm -felf32 boot.asm -o ${BIN_PATH}/boot.o
	@i686-elf-g++ -c kernel.cpp -o ${BIN_PATH}/kernel.o -I . -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
	@i686-elf-g++ -T linker.ld -o ${BIN_PATH}/kernel.elf -ffreestanding -O2 -nostdlib ${BIN_PATH}/boot.o ${BIN_PATH}/kernel.o ${BIN_PATH}/${FONT_NAME}.o -lgcc

iso:
	@mkdir -p tmp/boot/grub
	@cp ${BIN_PATH}/kernel.elf tmp/boot
	@cp grub/grub.cfg tmp/boot/grub
	@grub-mkrescue -o ${BIN_PATH}/kernel.iso tmp
	@rm -r tmp

clean:
	@rm -f ${BIN_PATH}/*

font:
	@objcopy -O elf32-i386 -B i386 -I binary assets/${FONT_NAME}.${FONT_EXT} ${BIN_PATH}/${FONT_NAME}.o

run:
	qemu-system-i386 -cdrom ${BIN_PATH}/${ISO_NAME}	

check: check_multi_boot.sh
	./check_multi_boot.sh
