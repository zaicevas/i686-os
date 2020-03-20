#ifndef IDT_H
#define IDT_H

#include <stdint.h>

namespace idt { 

	struct IDT_entry {
		unsigned short int offset_lowerbits;
		unsigned short int selector;
		unsigned char zero;
		unsigned char type_attr;
		unsigned short int offset_higherbits;
	};

    void init();
    void set_gate(uint8_t index, uint32_t address);
    IDT_entry *get_idt();

}

#endif
