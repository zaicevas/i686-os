bits 32

section .text

global start, read_port, write_port, load_idt, keyboard_handler

extern kernel_main
extern keyboard_handler_main

read_port:
	mov edx, [esp + 4]
			;al is the lower 8 bits of eax
	in al, dx	;dx is the lower 16 bits of edx
	ret

write_port:
	mov   edx, [esp + 4]    
	mov   al, [esp + 4 + 4]  
	out   dx, al  
	ret

load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	sti 				;turn on interrupts
	ret

keyboard_handler:                 
	call    keyboard_handler_main
	iretd

start:
	mov esp, stack_top

	push ebx			; multiboot2 information data structure
	call kernel_main

section .bss
align 16
	stack_bottom:
		resb 16384 ; 16 KiB
	stack_top:
