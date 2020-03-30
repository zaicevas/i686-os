#include <pic.h>
#include <system.h>
#include <idt.h>
#include <string.h>
#include <debug.h>
#include <terminal.h>

#define INTERRUPT_GATE 0x8E
#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_DATA 0xA1

using namespace idt;

namespace pic {

    void init() {
		/* ICW1 - begin initialization */
		outb(PIC1_COMMAND, 0x11);
		outb(0xA0 , 0x11);

		/* ICW2 - remap offset address of IDT */
		/*
		* In x86 protected mode, we have to remap the PICs beyond 0x20 because
		* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
		*/

		outb(PIC1_DATA, 0x20);
		outb(PIC2_DATA, 0x28);

		/* ICW3 - setup cascading */
		outb(PIC1_DATA, 0x00);
		outb(PIC2_DATA, 0x00);

		/* ICW4 - environment info */
		outb(PIC1_DATA, 0x01);
		outb(PIC2_DATA, 0x01);
		/* Initialization finished */

		/* mask interrupts */
		outb(PIC1_DATA, 0xFF);
		outb(PIC2_DATA, 0xFF);
    }

	void set_gate(uint8_t index, uint32_t address) {
		IDT_entry *IDT = get_idt();

		IDT[index].offset_lowerbits = address & 0xFFFF;
		IDT[index].selector = KERNEL_CODE_SEGMENT_OFFSET;
		IDT[index].zero = 0;
		IDT[index].type_attr = INTERRUPT_GATE;
		IDT[index].offset_higherbits = (address & 0xFFFF0000) >> 16;
	}

	// enable interrupt
	void unmask(uint8_t irq) {
		uint16_t port = irq > 8 ? PIC2_DATA : PIC1_DATA;

		uint8_t imr = inb(port); // interrupt mask register
		uint8_t new_imr = imr ^ (1 << irq); 

		outb(port, new_imr);
	}

	// disable interrupt
	void mask(uint8_t irq) {
		uint16_t port = irq > 8 ? PIC2_DATA : PIC1_DATA;

		uint8_t imr = inb(port); // interrupt mask register
		uint8_t new_imr = imr | (1 << irq); 

		outb(port, new_imr);
	}

}
