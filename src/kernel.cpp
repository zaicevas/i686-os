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

using namespace terminal;

/* Defines a GDT entry. We say packed, because it prevents the
*  compiler from doing things that it thinks is best: Prevent
*  compiler "optimization" by packing */
struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

/* Special pointer which includes the limit: The max bytes
*  taken up by the GDT, minus 1. Again, this NEEDS to be packed */
struct gdtr_t {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

/* Our GDT, with 3 entries, and finally our special GDT pointer */
struct gdt_entry gdt[3];
struct gdtr_t gdtr;

/* This will be a function in start.asm. We use this to properly
*  reload the new segment registers */
extern "C" void gdt_flush();

/* Setup a descriptor in the Global Descriptor Table */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran) {
    /* Setup the descriptor base address */
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install() {
    /* Setup the GDT pointer and limit */
    gdtr.limit = (sizeof(gdt_entry) * 3) - 1;
    gdtr.base = (uint32_t) &gdt;

    /* Our NULL descriptor */
    gdt_set_gate(0, 0, 0, 0, 0);

    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // CS
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // DS

    gdt_set_gate(3, 0, 0xFFFFFFFF, 0x89, 0xCF);

    gdt_flush();
}

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
	kprintf("Graphics initialized: %d x %d x %d\n", screen.width, screen.height, screen.bytes_per_pixel * 8);

	gdt_install();
	kprintf("GDT initialized\n");

	idt::init();
	kprintf("IDT initialized\n");

	keyboard::init();
	kprintf("Keyboard initialized\n");

	halt();

}
