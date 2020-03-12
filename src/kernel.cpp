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

	VGA_MODE vga_mode = terminal::get_vga_mode();

	if (vga_mode != VGA_MODE::GRAPHICS) {
		kprintf("Only graphics mode is supported by the OS.");
		return;
	}

	canvas_t screen = terminal::get_screen_canvas();

	kprintf("Graphics initialized: %d x %d x %d", screen.width, screen.height, screen.bytes_per_pixel * 8);

}
