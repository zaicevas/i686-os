# i686-os
Toy OS for i686 arch

## Usage
```
make # builds .iso and runs qemu
make clean build # builds .iso
make clean build run_debug # builds .iso and runs qemu with gdb stub
```

## Features
- Multiboot 2 compliant
- Graphics mode (VESA)

### Hardware requirements
- Should support graphics mode and linear framebuffer

### Useful debugging cmds
```
objdump -D kernel.o -M intel
objdump -s kernel.o -M intel
readelf -W -s kernel.o		// -W tells not to truncate names
g++ -O0
g++ -S -masm=intel -o kernel.asm
```

### Other useful stuff
- QEMU framebuffer address 0xFD000000 (https://wiki.osdev.org/Drawing_In_Protected_Mode)
- Writing to a USB flash drive `sudo dd if=myos.iso of=/dev/sdx && sync`

### TODO
- Change #define to const where appropriate
