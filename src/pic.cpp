#include <pic.h>
#include <system.h>
#include <idt.h>

#define INTERRUPT_GATE 0x8E
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

using namespace idt;

namespace pic {

    void init() {
		/*     Ports
		*	 PIC1	PIC2
		*Command 0x20	0xA0
		*Data	 0x21	0xA1
		*/

		/* ICW1 - begin initialization */
		outb(0x20 , 0x11);
		outb(0xA0 , 0x11);

		/* ICW2 - remap offset address of IDT */
		/*
		* In x86 protected mode, we have to remap the PICs beyond 0x20 because
		* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
		*/
		outb(0x21 , 0x20);
		outb(0xA1 , 0x28);

		/* ICW3 - setup cascading */
		outb(0x21 , 0x00);
		outb(0xA1 , 0x00);

		/* ICW4 - environment info */
		outb(0x21 , 0x01);
		outb(0xA1 , 0x01);
		/* Initialization finished */

		/* mask interrupts */
		outb(0x21 , 0xFF);
		outb(0xA1 , 0xFF);
    }

	void set_gate(uint8_t index, uint32_t address) {
        IDT_entry *IDT = get_idt();

		IDT[index].offset_lowerbits = address & 0xFFFF;
		IDT[index].selector = KERNEL_CODE_SEGMENT_OFFSET;
		IDT[index].zero = 0;
		IDT[index].type_attr = INTERRUPT_GATE;
		IDT[index].offset_higherbits = (address & 0xFFFF0000) >> 16;
	}

}