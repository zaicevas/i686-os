#include <stdint.h>
#include <system.h>
#include <terminal.h>
#include <debug.h>
#include <string.h>
#include <keyboard.h>

#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8E
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C

namespace idt {

	void load_lidt(void *base, uint16_t size) { 
	    struct {
			uint16_t length;
			void *base;
	    } __attribute__((packed)) IDTR = { size, base };
	 
		asm volatile ("sti");
		asm volatile ( "lidt %0" : : "m"(IDTR) );
	}

	struct IDT_entry {
		unsigned short int offset_lowerbits;
		unsigned short int selector;
		unsigned char zero;
		unsigned char type_attr;
		unsigned short int offset_higherbits;
	};

	IDT_entry IDT[IDT_SIZE];

	void init() {
		memset((uint8_t *) &IDT, 0, sizeof(IDT_entry) * IDT_SIZE);

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

		/* fill the IDT descriptor */
		const uint16_t size = (sizeof (IDT_entry) * IDT_SIZE) + (((uint32_t) IDT & 0xFFFF) << 16);
		
		load_lidt((void *) IDT, size);
	}

	void set_gate(uint8_t index, uint32_t address) {
		IDT[index].offset_lowerbits = address & 0xFFFF;
		IDT[index].selector = KERNEL_CODE_SEGMENT_OFFSET;
		IDT[index].zero = 0;
		IDT[index].type_attr = INTERRUPT_GATE;
		IDT[index].offset_higherbits = (address & 0xFFFF0000) >> 16;
	}

}
