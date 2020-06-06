#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

extern "C" void *kernel_starts_at;
extern "C" void *kernel_ends_at;

namespace memory { 
    void init(uint64_t multiboot_addr);

    /* prints where kernel binary resides in memory */
    void print_kernel_memory();

    uint16_t get_available_ram_mb();
}

#endif
