#include <gdt.h>

namespace gdt {

    struct gdt_entry {
        unsigned short limit_low;
        unsigned short base_low;
        unsigned char base_middle;
        unsigned char access;
        unsigned char granularity;
        unsigned char base_high;
    } __attribute__((packed));

    struct gdtr_t {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed));

    struct gdt_entry gdt[5];
    
    // points to gdt entries
    struct gdtr_t gdtr;

    void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran) {
        gdt[num].base_low = (base & 0xFFFF);
        gdt[num].base_middle = (base >> 16) & 0xFF;
        gdt[num].base_high = (base >> 24) & 0xFF;

        gdt[num].limit_low = (limit & 0xFFFF);
        gdt[num].granularity = ((limit >> 16) & 0x0F);

        gdt[num].granularity |= (gran & 0xF0);
        gdt[num].access = access;
    }

    void gdt_load() {
		asm volatile ("lgdt %0" : : "m"(gdtr));

        asm volatile ("mov ax, 0x10     \n \
            mov ds, ax \n \
            mov es, ax \n \
            mov fs, ax \n \
            mov gs, ax \n \
            mov ss, ax \n \
            jmp 0x08:1f \n \   
            1: \n");

    }

    void init() {
        gdtr.limit = (sizeof(gdt_entry) * 5) - 1;
        gdtr.base = (uint32_t) &gdt;

        /* NULL descriptor */
        gdt_set_gate(0, 0, 0, 0, 0);

        gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // CS
        gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // DS
        // http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
        gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
        gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

        //gdt_set_gate(&tss, 0, 0xFFFFFFFF, 0x89, 0xCF); // TSS, todo

        gdt_load();
    }
}