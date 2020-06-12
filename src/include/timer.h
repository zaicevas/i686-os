#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER_HZ_FREQUENCY 100

namespace timer {
    // ESP and SS could be wrong, need to double check
    struct registers_t {
        uint32_t esi, edi, edx, ecx, ebp, ebx, eax, eip, cs, eflags, esp, ss;
    };

    void init();
    uint64_t get_ticks();
}

#endif
