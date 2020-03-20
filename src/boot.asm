bits 32

section .text

global start
extern kmain, gdtr

start:
	mov esp, stack_top

	push ebx			; pass multiboot2 data as a parameter 
	call kmain

section .bss
align 16
	stack_bottom:
		resb 16384 ; 16 KiB
	stack_top:
