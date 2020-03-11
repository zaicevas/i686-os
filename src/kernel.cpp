#include <stdint.h>
#include <terminal.h>
#include <debug.h>
#include <multiboot2.h>
#include <system.h>
#include <keyboard.h>
#include <string.h>

using namespace terminal;

#if defined(__linux__)
	#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
	#error "This code must be compiled with an x86-elf compiler"
#endif

#if defined(__cplusplus)
	extern "C"
#endif
void kernel_main(uint64_t addr) {
	multiboot_framebuffer *framebuffer = get_framebuffer(addr);

	if (framebuffer)
		terminal::init(*framebuffer);
	else {
		qemu_printf("multiboot_framebuffer not found");
		return;
	}

	if (terminal::get_vga_mode() != VGA_MODE::GRAPHICS) {
		print_text("Only graphics mode is supported by the OS.");
		return;
	}

	print_text("framebuffer_type: ");
	print_text(itoa(framebuffer->framebuffer_type));
	print_text(", framebuffer_width: ");
	print_text(itoa(framebuffer->framebuffer_width));
	print_text(", framebuffer_height: ");
	print_text(itoa(framebuffer->framebuffer_height));
}
