bits 32

section .text

global start, gdt_flush
extern kernel_main, gdtr

start:
	mov esp, stack_top

	push ebx			; pass multiboot2 data to kernel_main
	call kernel_main

gdt_flush:
    lgdt [gdtr]
    mov ax, 0x10      
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush2   
flush2:
    ret               

section .bss
align 16
	stack_bottom:
		resb 16384 ; 16 KiB
	stack_top:
