FONT_PATH = assets/zap-vga16.psf

CC = i686-elf-g++
BIN_PATH = ../bin
CFLAGS = -I include -c -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
ASM_FLAGS = -felf32
LINKER_FLAGS = -T ${LINKER_PATH} -ffreestanding -O2 -nostdlib -lgcc
LINKER_PATH = linker.ld
INCLUDE_PATH = include

KERNEL = ${BIN_PATH}/kernel.elf
ISO_KERNEL = kernel.iso

CPP_FILES := ${wildcard *.cpp}
ASM_FILES := ${wildcard *.asm}
OBJ_FILES := ${patsubst %.cpp, ../bin/%.o, ${CPP_FILES}} \
	${patsubst %.asm, ../bin/%.o, ${ASM_FILES}}

.PHONY: all build clean run run_log_file check font 

all: clean build run

build: ${KERNEL} ${ISO_KERNEL}

clean:
	@rm -rf ${BIN_PATH}

run: ${ISO_KERNEL}
	@qemu-system-i386 -cdrom ${BIN_PATH}/${ISO_KERNEL} -serial stdio 

run_log_file:
	qemu-system-i386 -cdrom ${BIN_PATH}/${ISO_KERNEL} -serial file:debug.log

check: check_multi_boot.sh ${KERNEL}
	@./check_multi_boot.sh

font: ${FONT_PATH}
	@mkdir -p ${BIN_PATH}
	@objcopy -O elf32-i386 -B i386 -I binary ${FONT_PATH} ${BIN_PATH}/${FONT_NAME}.o


${KERNEL}: font ${OBJ_FILES} ${LINKER_PATH}
	@mkdir -p ${BIN_PATH}
	@${CC} -o ${KERNEL} ${LINKER_FLAGS} ${OBJ_FILES}

${ISO_KERNEL}: ${KERNEL}
	@mkdir -p tmp/boot/grub
	@cp ${BIN_PATH}/kernel.elf tmp/boot
	@cp grub/grub.cfg tmp/boot/grub
	@grub-mkrescue -o ${BIN_PATH}/kernel.iso tmp
	@rm -r tmp

${BIN_PATH}/%.o: %.asm
	@mkdir -p ${BIN_PATH}
	@nasm ${ASM_FLAGS} $< -o $@

${BIN_PATH}/%.o: %.cpp
	@mkdir -p ${BIN_PATH}
	@${CC} $< -o $@ ${CFLAGS}
