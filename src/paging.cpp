#include <paging.h>
#include <terminal.h>
#include <system.h>
#include <memory.h>
#include <pic.h>
#include <string.h>
#include <debug.h>

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

using namespace terminal;
using namespace memory;

typedef unsigned int uword_t __attribute__ ((mode (__word__)));

namespace paging {

    uint32_t page_directory[1024] __attribute__((aligned(4096)));
    uint32_t first_page_table[1024] __attribute__((aligned(4096)));

    uint64_t placement_address = 0;

    // The kernel's page directory
    page_directory_t *kernel_directory = 0;

    // The current page directory;
    page_directory_t *current_directory = 0;

    // A bitset of frames - used or free.
    uint32_t *frames;
    uint32_t nframes;

    // Macros used in the bitset algorithms.

    // Static function to set a bit in the frames bitset
    static void set_frame(uint32_t frame_addr) {
        uint32_t frame = frame_addr/0x1000;
        uint32_t idx = INDEX_FROM_BIT(frame);
        uint32_t off = OFFSET_FROM_BIT(frame);
        frames[idx] |= (0x1 << off);
    }

    // Static function to clear a bit in the frames bitset
    static void clear_frame(uint32_t frame_addr) {
        uint32_t frame = frame_addr/0x1000;
        uint32_t idx = INDEX_FROM_BIT(frame);
        uint32_t off = OFFSET_FROM_BIT(frame);
        frames[idx] &= ~(0x1 << off);
    }

    // Static function to test if a bit is set.
    static uint32_t test_frame(uint32_t frame_addr) {
        uint32_t frame = frame_addr/0x1000;
        uint32_t idx = INDEX_FROM_BIT(frame);
        uint32_t off = OFFSET_FROM_BIT(frame);
        return (frames[idx] & (0x1 << off));
    }

    // Static function to find the first free frame.
    static uint32_t first_frame() {
        uint32_t i, j;
        for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
            if (frames[i] != 0xFFFFFFFF) { // nothing free, exit early.
                // at least one bit is free here.
                for (j = 0; j < 32; j++) {
                    uint32_t toTest = 0x1 << j;
                    if ( !(frames[i]&toTest) ) {
                        return i*4*8+j;
                    }
                }
            }
        }
    }

    // Function to allocate a frame.
    void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
        if (page->frame != 0) {
            return;
        }
        else {
            uint32_t idx = first_frame();
            if (idx == (uint32_t)-1) {
                PANIC("No free frames!");
            }
            set_frame(idx*0x1000);
            // qemu_printf("setting frame at: ");
            // qemu_printf(itoa(idx * 0x1000));
            // qemu_printf("\n");
            page->present = 1;
            page->rw = (is_writeable)?1:0;
            page->user = (is_kernel)?0:1;
            page->frame = idx;
        }
    }

    // Function to deallocate a frame.
    void free_frame(page_t *page) {
        uint32_t frame;
        if (!(frame=page->frame)) {
            return;
        }
        else {
            clear_frame(frame);
            page->frame = 0x0;
        }
    }

    void init() {
        paging::placement_address = (uint32_t) &kernel_ends_at;

        unsigned int i;

        memset((uint8_t*)page_directory, 0, 4 * 1024);
        memset((uint8_t*)first_page_table, 0, 4 * 1024);
        
        //we will fill all 1024 entries in the table, mapping 4 megabytes
        for (i = 0; i < 1024; i++) {
            // As the address is page aligned, it will always leave 12 bits zeroed.
            // Those bits are used by the attributes ;)
            first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
        }

        page_directory[0] = ((unsigned int)first_page_table) | 3;

/*
        // The size of physical memory. For the moment we 
        // assume it is 16MB big.
        // TODO: find the end of RAM
        uint32_t mem_end_page = 0x1000000;
        
        nframes = mem_end_page / 0x1000;
        frames = (uint32_t*) kmalloc(INDEX_FROM_BIT(nframes));
        memset((uint8_t*) frames, 0, INDEX_FROM_BIT(nframes));
        
        // Let's make a page directory.
        kernel_directory = (page_directory_t*) kmalloc_a(sizeof(page_directory_t));
        current_directory = kernel_directory;

        // We need to identity map (phys addr = virt addr) from
        // 0x0 to the end of used memory, so we can access this
        // transparently, as if paging wasn't enabled.
        // NOTE that we use a while loop here deliberately.
        // inside the loop body we actually change placement_address
        // by calling kmalloc(). A while loop causes this to be
        // computed on-the-fly rather than once at the start.
        // uint32_t i = 0;
        // while (i < placement_address * 2) {
        //     // Kernel code is readable but not writeable from userspace.
        //     alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
        //     i += 0x1000;
        //     qemu_printf("i: ");
        //     qemu_printf(itoa(i));
        //     qemu_printf("\n");
        // }
        // Before we enable paging, we must register our page fault handler.
        // pic::set_gate(14, (uint32_t) &page_fault);

        */
        pic::register_interrupt_handler(14, (uint32_t) page_fault);

        // Now, enable paging!
        switch_page_directory(kernel_directory);
    }

    void switch_page_directory(page_directory_t *dir) {
        // current_directory = dir;
        asm volatile("mov %%cr3, %0":: "r"(page_directory)); // write to cr3
        // while(true) {}
        uint32_t cr0 = read_cr0();
        cr0 |= 0x80000000; // Enable paging!
        write_cr0(cr0);
    }

    page_t *get_page(uint32_t address, int make, page_directory_t *dir) {
        // Turn the address into an index.
        address /= 0x1000;
        // Find the page table containing this address.
        uint32_t table_idx = address / 1024;
        // if (dir->tables[table_idx]) { // If this table is already assigned
        // qemu_printf("address: ");
        // qemu_printf(itoa(address));
        // qemu_printf("\n");
        //     // qemu_printf("already assigned");
        //     // qemu_printf(itoa((uint32_t) &dir->tables[table_idx]->pages[address%1024]));
        //     return &dir->tables[table_idx]->pages[address%1024];
        // }
        if (make) {
            qemu_printf("what");
            uint32_t tmp;
            dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
            qemu_printf("yes im there");
            dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
            return &dir->tables[table_idx]->pages[address%1024];
        }
        else {
            return 0;
        }
    }


    void page_fault(uword_t error_code) {
        // A page fault has occurred.
        // The faulting address is stored in the CR2 register.
        uint32_t faulting_address;
        asm volatile("mov %0, %%cr2" : "=r" (faulting_address));
        
        // The error code gives us details of what happened.
        int present = !(error_code & 0x1); // Page not present
        int rw = error_code & 0x2;           // Write operation?
        int us = error_code & 0x4;           // Processor was in user-mode?
        int reserved = error_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
        int id = error_code & 0x10;          // Caused by an instruction fetch?

        // Output an error message.
        kprintf("Page fault! ( ");
        if (present) {kprintf("present ");}
        if (rw) {kprintf("read-only ");}
        if (us) {kprintf("user-mode ");}
        if (reserved) {kprintf("reserved ");}
        kprintf(") at 0x%x\n", faulting_address);
        PANIC("Page fault");
    }

}