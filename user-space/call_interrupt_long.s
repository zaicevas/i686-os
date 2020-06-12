bits 32

mov eax, 0
mov ecx, 15000

do_loop:
	mov eax, ecx
	int 0x80
	loop do_loop

int 0x81
jmp $
ret
