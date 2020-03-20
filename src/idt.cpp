#include <stdint.h>
#include <idt.h>
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

	IDT_entry IDT[IDT_SIZE];

	void init() {
		memset((uint8_t *) &IDT, 0, sizeof(IDT_entry) * IDT_SIZE);

		const uint16_t size = (sizeof (IDT_entry) * IDT_SIZE) + (((uint32_t) IDT & 0xFFFF) << 16);
		
		load_lidt((void *) IDT, size);
	}

	IDT_entry *get_idt() {
		return IDT;
	}

}
