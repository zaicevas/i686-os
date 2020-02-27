extern kernel_main

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
