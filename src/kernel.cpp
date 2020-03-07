#include <stdint.h>
#include <font.h>
#include <debug.h>
#include <multiboot2.h>

#if defined(__linux__)
	#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
	#error "This code must be compiled with an x86-elf compiler"
#endif

#if defined(__cplusplus)
	extern "C"
#endif
int kernel_main(uint64_t addr) {
	multiboot_framebuffer *framebuffer = get_framebuffer(addr);

	if (framebuffer)
		init_vga_mode(*framebuffer);
	else
		qemu_printf("multiboot_framebuffer not found");

	print_text("Hello, world!\nHello, second world!");

	return 0;
}
