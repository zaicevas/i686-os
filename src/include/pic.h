#ifndef PIC_H
#define PIC_H

#define END_OF_INTERRUPT outb(0x20, 0x20);

#include <stdint.h>

namespace pic {
	void init();
    void set_gate(uint8_t index, uint32_t address); 
    void unmask(uint8_t irq); // irq = maskable hardware interrupt [0, 15]
    void mask(uint8_t irq); 
}

#endif
