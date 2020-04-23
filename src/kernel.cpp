
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

	multiboot_tag_mmap *mmap = get_memory_map(multiboot_addr);
	kprintf("Memory areas available for OS:\n");

	uint64_t ram = 0;
	uint64_t mega_byte = 0x100000;
		
	multiboot_memory_map *map; 
	for (map = mmap->entries; (uint8_t*) map < (uint8_t*) mmap + mmap->size;
		map = (multiboot_memory_map *) ((uint32_t) map + ((multiboot_tag_mmap *) mmap)->entry_size)) {
			if (map->type == MULTIBOOT_MEMORY_AVAILABLE) {
				terminal::kprintf("[0x%x, 0x%x]\n", map->addr, (uint64_t) (map->addr + map->len));
				ram += map->len;
			}
	}

	kprintf("RAM: %uMB\n", ram / mega_byte);

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

	// kprintf("Lower memory has %x kb\n", meminfo->mem_lower);
	// kprintf("Upper memory has %x kb\n", meminfo->mem_upper);

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
