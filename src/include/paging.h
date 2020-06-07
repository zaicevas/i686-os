#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

extern "C" void *kernel_ends_at;

typedef unsigned int uword_t __attribute__ ((mode (__word__)));

namespace paging {

    struct registers_t {
        uint32_t ds;                  // Data segment selector
        uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
        uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
        uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
    };

    struct page_t {
        uint32_t present    : 1;   // Page present in memory
        uint32_t rw         : 1;   // Read-only if clear, readwrite if set
        uint32_t user       : 1;   // Supervisor level only if clear
        uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
        uint32_t dirty      : 1;   // Has the page been written to since last refresh?
        uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
        uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
    };

    struct page_table_t {
        page_t pages[1024];
    };

    struct page_directory_t {
        /**
             Array of pointers to pagetables.
        **/
        page_table_t *tables[1024];
        /**
             Array of pointers to the pagetables above, but gives their *physical*
            location, for loading into the CR3 register.
        **/
        uint32_t tablesPhysical[1024];
        /**
             The physical address of tablesPhysical. This comes into play
            when we get our kernel heap allocated and the directory
            may be in a different location in virtual memory.
        **/
        uint32_t physicalAddr;
    };

    /**
     Sets up the environment, page directories etc and
    enables paging.
    **/
    void init();

    /**
     Causes the specified page directory to be loaded into the
    CR3 register.
    **/
    void switch_page_directory(page_directory_t *next);

    /**
     Retrieves a pointer to the page required.
    If make == 1, if the page-table in which this page should
    reside isn't created, create it!
    **/
    page_t *get_page(uint32_t address, int make, page_directory_t *dir);

    /**
     Handler for page faults.
    **/
    void page_fault(uword_t error_code);

}

#endif