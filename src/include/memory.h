#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

extern "C" void *kernel_starts_at;
extern "C" void *kernel_ends_at;

namespace memory { 
    void init(uint64_t multiboot_addr);

    void update_placement_address(uint32_t furthest_file_address);

    /* prints where kernel binary resides in memory */
    void print_kernel_memory();

    uint16_t get_available_ram_mb();

    // http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html
    /**
     Allocate a chunk of memory, sz in size. If align == 1,
    the chunk must be page-aligned. If phys != 0, the physical
    location of the allocated chunk will be stored into phys.

    This is the internal version of kmalloc. More user-friendly
    parameter representations are available in kmalloc, kmalloc_a,
    kmalloc_ap, kmalloc_p.
    **/
    uint32_t kmalloc_int(uint32_t sz, int align, uint32_t *phys);

    /**
     Allocate a chunk of memory, sz in size. The chunk must be
    page aligned.
    **/
    uint32_t kmalloc_a(uint32_t sz);

    /**
     Allocate a chunk of memory, sz in size. The physical address
    is returned in phys. Phys MUST be a valid pointer to uint32_t!
    **/
    uint32_t kmalloc_p(uint32_t sz, uint32_t *phys);

    /**
     Allocate a chunk of memory, sz in size. The physical address 
    is returned in phys. It must be page-aligned.
    **/
    uint32_t kmalloc_ap(uint32_t sz, uint32_t *phys);

    /**
     General allocation function.
    **/
    uint32_t kmalloc(uint32_t sz);
}

#endif
