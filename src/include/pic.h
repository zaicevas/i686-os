
#ifndef PIC_H
#define PIC_H

#include <stdint.h>

namespace pic {
	void init();
    void set_gate(uint8_t index, uint32_t address); 
}

#endif
