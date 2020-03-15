bits 32

section .text

global start, gdt_flush

extern kernel_main, gdtr

gdt_flush:
    lgdt [gdtr]       ; Load the GDT with our '_gdtr' which is a special pointer
    mov ax, 0x08      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x10:flush2   ; 0x08 is the offset to our code segment: Far jump!
flush2:
    ret               ; Returns back to the C code!

start:
	mov esp, stack_top

	push ebx			; multiboot2 information data structure
	call kernel_main

section .bss
align 16
	stack_bottom:
		resb 16384 ; 16 KiB
	stack_top:
