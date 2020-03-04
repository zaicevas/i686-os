##FONT_NAME = 'Goha-16'
FONT_NAME = 'zap-vga16'
FONT_EXT = 'psf'
BIN_PATH = '../bin'
INCLUDE_PATH = 'include'
ISO_NAME = 'kernel.iso'

install: compile iso

compile: boot.asm kernel.cpp font.cpp
	@mkdir -p ${BIN_PATH}
	${MAKE} font
	@nasm -felf32 boot.asm -o ${BIN_PATH}/boot.o
	@i686-elf-g++ string.cpp -I ${INCLUDE_PATH} -o ${BIN_PATH}/string.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -c -lgcc
	@i686-elf-g++ debug.cpp -I ${INCLUDE_PATH} -o ${BIN_PATH}/debug.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -c -lgcc
	@i686-elf-g++ multiboot2.cpp -I ${INCLUDE_PATH} -o ${BIN_PATH}/multiboot2.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -c -lgcc
	@i686-elf-g++ font.cpp -I ${INCLUDE_PATH} -o ${BIN_PATH}/font.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -c -lgcc
	@i686-elf-g++ font_parser.cpp -I ${INCLUDE_PATH} -o ${BIN_PATH}/font_parser.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -c -lgcc
	@i686-elf-g++ -c kernel.cpp -o ${BIN_PATH}/kernel.o -I ${INCLUDE_PATH} -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -lgcc
	i686-elf-g++ -T linker.ld -o ${BIN_PATH}/kernel.elf -ffreestanding -O2 -nostdlib -lgcc \
		${BIN_PATH}/boot.o \
		/home/fern/os/cross/src/build-gcc/i686-elf/libgcc/_udivdi3.o \
		/home/fern/os/cross/src/build-gcc/i686-elf/libgcc/_umoddi3.o \
		${BIN_PATH}/multiboot2.o \
		${BIN_PATH}/kernel.o \
		${BIN_PATH}/debug.o \
		${BIN_PATH}/${FONT_NAME}.o \
		${BIN_PATH}/string.o \
		${BIN_PATH}/font_parser.o \
		${BIN_PATH}/font.o

iso:
	@mkdir -p tmp/boot/grub
	@cp ${BIN_PATH}/kernel.elf tmp/boot
	@cp grub/grub.cfg tmp/boot/grub
	@grub-mkrescue -o ${BIN_PATH}/kernel.iso tmp
	@rm -r tmp

clean:
	@rm -rf ${BIN_PATH}

font:
	@mkdir -p ${BIN_PATH}
	@objcopy -O elf32-i386 -B i386 -I binary assets/${FONT_NAME}.${FONT_EXT} ${BIN_PATH}/${FONT_NAME}.o

run:
	##qemu-system-i386 -cdrom ${BIN_PATH}/${ISO_NAME} -serial file:serial.log
	##qemu-system-i386 -cdrom ${BIN_PATH}/${ISO_NAME} -serial stdio -vga cirrus --cpu base
	qemu-system-i386 -cdrom ${BIN_PATH}/${ISO_NAME} -serial stdio 

check: check_multi_boot.sh
	./check_multi_boot.sh
