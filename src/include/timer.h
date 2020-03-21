#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER_HZ_FREQUENCY 100

namespace timer {
    void init();
    uint64_t get_ticks();
}

#endif
