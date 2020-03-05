# i686-os
Toy OS for i686 arch

## Dependencies
- nasm
- QEMU (qemu-system-i386)
- GCC cross-compiler for i686 in $PATH

## Usage
```
make 
```

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
