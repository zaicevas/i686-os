bits 32

section .text

global start, timer_interrupt_handler
extern kmain, call_scheduler

start:
	mov esp, stack_top

	push ebx			; pass multiboot2 data as a parameter 
	call kmain

timer_interrupt_handler:
	push eax
	push ebx
	push ebp
	push ecx
	push edx
	push edi
	push esi
	mov eax, esp
	push eax
	call call_scheduler
	pop eax
	pop esi
	pop edi
	pop edx
	pop ecx
	pop ebp
	pop ebx
	pop eax
	iretd

section .bss
align 16
	stack_bottom:
		resb 16384 ; 16 KiB
	stack_top:
