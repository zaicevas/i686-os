#ifndef IDT_H
#define IDT_H

#include <stdint.h>

namespace idt { 
    void init();
    void set_gate(uint8_t index, uint32_t address);
}

#endif
