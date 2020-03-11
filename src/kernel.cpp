#include <stdint.h>
#include <terminal.h>
#include <debug.h>
#include <multiboot2.h>
#include <system.h>
#include <keyboard.h>
#include <string.h>

using namespace terminal;

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

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

	enable_cursor(0, 15);
	move_cursor(2, 0);

	if (framebuffer)
		terminal::init(*framebuffer);
	else
		qemu_printf("multiboot_framebuffer not found");

	print_text("framebuffer_type: ");
	print_text(itoa(framebuffer->framebuffer_type));
	print_text(", framebuffer_width: ");
	print_text(itoa(framebuffer->framebuffer_width));
	print_text(", framebuffer_bpp: ");
	print_text(itoa(framebuffer->framebuffer_bpp));

	return 0;
}
