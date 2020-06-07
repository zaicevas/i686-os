#ifndef IDT_H
#define IDT_H

#include <stdint.h>

namespace idt { 

	struct IDT_entry {
		unsigned short int offset_lowerbits;
		unsigned short int selector;
		unsigned char zero;
		unsigned char type_attr; // flags
		unsigned short int offset_higherbits;
	};

    void init();
    IDT_entry *get_idt();

}

#endif
