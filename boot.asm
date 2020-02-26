extern kernel_main

MB_ALIGN    equ  1 << 0         
MB_MEMINFO  equ  1 << 1          
MB_FLAGS    equ  MB_ALIGN | MB_MEMINFO
MB_MAGIC    equ  0x1BADB002        
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS)

section .multiboot
align 4
	dd MB_MAGIC
	dd MB_FLAGS
	dd MB_CHECKSUM

section .bss
align 16
	stack_bottom:
		resb 16384 ; 16 KiB
	stack_top:

section .text
global _start:function (_start.end - _start)

_start:
	mov esp, stack_top
	call kernel_main
.hang:
	cli
	hlt
	jmp .hang
.end:
