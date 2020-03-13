bits 32

section .text

;global start, keyboard_handler
global start

extern kernel_main
;extern keyboard_handler_main

;keyboard_handler:                 
;	call keyboard_handler_main
;	iretd

start:
	mov esp, stack_top

	push ebx			; multiboot2 information data structure
	call kernel_main

section .bss
align 16
	stack_bottom:
		resb 16384 ; 16 KiB
	stack_top:
