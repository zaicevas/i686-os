#include <stdint.h>
#include <system.h>
#include <terminal.h>
#include <debug.h>
#include <string.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8E
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C

uint8_t keyboard_map[128] = {
		0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
	'9', '0', '-', '=', '\b',	/* Backspace */
	'\t',			/* Tab */
	'q', 'w', 'e', 'r',	/* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
		0,			/* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
	'\'', '`',   0,		/* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
	'm', ',', '.', '/',   0,				/* Right shift */
	'*',
		0,	/* Alt */
	' ',	/* Space bar */
		0,	/* Caps lock */
		0,	/* 59 - F1 key ... > */
		0,   0,   0,   0,   0,   0,   0,   0,
		0,	/* < ... F10 */
		0,	/* 69 - Num lock*/
		0,	/* Scroll Lock */
		0,	/* Home key */
		0,	/* Up Arrow */
		0,	/* Page Up */
	'-',
		0,	/* Left Arrow */
		0,
		0,	/* Right Arrow */
	'+',
		0,	/* 79 - End key*/
		0,	/* Down Arrow */
		0,	/* Page Down */
		0,	/* Insert Key */
		0,	/* Delete Key */
		0,   0,   0,
		0,	/* F11 Key */
		0,	/* F12 Key */
		0,	/* All other keys are undefined */
};

namespace interrupt {

	void load_lidt(void *base, uint16_t size) { 
	    struct {
			uint16_t length;
			void *base;
	    } __attribute__((packed)) IDTR = { size, base };
	 
		asm volatile ("sti");
		asm volatile ( "lidt %0" : : "m"(IDTR) );
	}

	void kbd_ack() {
		while(!(inb(0x60)==0xfa));
	}


	void kbd_led_handling(uint8_t ledstatus){;
		outb(0x60,0xed);
		kbd_ack();
		outb(0x60, ledstatus);
	}

	struct interrupt_frame;

	bool caps_lock_led = false;
 
	__attribute__((interrupt)) void keyboard_interrupt_handler(interrupt_frame *frame) {
		outb(0x20, 0x20);
		const uint8_t status = inb(KEYBOARD_STATUS_PORT);
		
		if (status & 0x01) {
			uint8_t key = inb(KEYBOARD_DATA_PORT);

			if (key == 0x3A) {
				kbd_led_handling(caps_lock_led ? 0b00000100 : 0);
			}

			if ((key < 0 || key >= 54) && key != 57)
				return;

			terminal::kprintf("%c", keyboard_map[(unsigned char) key]);
		}
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
		unsigned long keyboard_address;

		/* populate IDT entry of keyboard's interrupt */
		keyboard_address = (unsigned long) keyboard_interrupt_handler;

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
		outb(0x21 , 0xff);
		outb(0xA1 , 0xff);

		/* fill the IDT descriptor */
		const uint16_t size = (sizeof (struct IDT_entry) * IDT_SIZE) + (((uint32_t) IDT & 0xffff) << 16);
		
		load_lidt((void *) IDT, size);

		IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
		IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
		IDT[0x21].zero = 0;
		IDT[0x21].type_attr = INTERRUPT_GATE;
		IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;
	}

}
