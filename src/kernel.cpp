#include <stdint.h>
#include <terminal.h>
#include <debug.h>
#include <multiboot2.h>
#include <system.h>
#include <keyboard.h>
#include <string.h>
#include <system.h>
#include <idt.h>
#include <keyboard.h>
#include <gdt.h>
#include <pic.h>
#include <timer.h>

using namespace terminal;

#if defined(__linux__)
	#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
	#error "This code must be compiled with an x86-elf compiler"
#endif

bool cpu_has_MSR();
void enable_cache();

extern "C"
void kmain(uint64_t multiboot_addr) {
	multiboot_framebuffer *framebuffer = get_framebuffer(multiboot_addr);
	multiboot_basic_memory_information *meminfo = get_basic_meminfo(multiboot_addr);

	if (framebuffer)
		terminal::init(*framebuffer);
	else {
		qemu_printf("multiboot_framebuffer not found");
		halt();
	}

	VGA_MODE vga_mode = terminal::get_vga_mode();

	if (vga_mode != VGA_MODE::GRAPHICS) {
		kprintf("Only graphics mode is supported by the OS.");
		halt();
	}

	canvas_t screen = terminal::get_screen_canvas();
	kprintf("Graphics initialized: %d x %d x %d\n", screen.width, screen.height, screen.bytes_per_pixel * 8);

	gdt::init();
	kprintf("GDT initialized\n");

	idt::init();
	kprintf("IDT initialized\n");

	pic::init();
	kprintf("PIC initialized\n");

	keyboard::init();
	kprintf("PS/2 Keyboard initialized\n");

	timer::init();
	kprintf("PIT ticks initialized\n");

	kprintf("Lower memory has %x kb\n", meminfo->mem_lower);
	kprintf("Upper memory has %x kb\n", meminfo->mem_upper);

	kprintf("MSR support: %s\n", cpu_has_MSR ? "yes" : "no");

	enable_cache();
	kprintf("CPU Cache: enabled\n");

	terminal::init_user_shell();

	halt();

}

bool cpu_has_MSR() {
	const uint32_t CPUID_FLAG_MSR = 1 << 5;
 
	uint32_t ax, dx;
	cpuid(1, &ax, &dx);
	return dx & CPUID_FLAG_MSR;
}

void enable_cache() {
	uint32_t cr0 = read_cr0();
	uint32_t new_cr0 = cr0 & 0x9FFFFFFF; // clearing 29 and 30 bits
	write_cr0(new_cr0);
}
