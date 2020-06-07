#include <memory.h>
#include <terminal.h>
#include <multiboot2.h>
#include <stdint.h>
#include <debug.h>
#include <string.h>

using namespace terminal;

namespace memory {
    const uint64_t MEGA_BYTE = 0x100000;

    uint64_t multiboot_addr = 0;
    uint64_t placement_address = 0;

    void init(uint64_t multiboot_addr) {
        memory::placement_address = (uint64_t) &kernel_ends_at;
        memory::multiboot_addr = multiboot_addr;
    }

    void print_kernel_memory() {
        uint64_t kernel_memory_start = (uint64_t) &kernel_starts_at;
        uint64_t kernel_memory_end = (uint64_t) &kernel_ends_at;
        kprintf("Kernel in memory starts at: 0x%x\n", kernel_memory_start);
        kprintf("Kernel in memory ends at: 0x%x\n", kernel_memory_end);   
    }

    uint16_t get_available_ram_mb() {
        multiboot_tag_mmap *mmap = get_memory_map(multiboot_addr);
        multiboot_memory_map *map; 
        uint64_t ram = 0;
        for (map = mmap->entries; (uint8_t*) map < (uint8_t*) mmap + mmap->size;
            map = (multiboot_memory_map *) ((uint32_t) map + ((multiboot_tag_mmap *) mmap)->entry_size)) {
                if (map->type == MULTIBOOT_MEMORY_AVAILABLE) {
                    // kprintf("[0x%x, 0x%x] sum: %luMB\n", map->addr, (uint64_t) (map->addr + map->len), map->len / MEGA_BYTE);
                    ram += map->len;
                }
        }
        return (uint16_t) (ram / MEGA_BYTE);
    }

    uint32_t kmalloc_int(uint32_t sz, int align, uint32_t *phys) {
        // This will eventually call malloc() on the kernel heap.
        // For now, though, we just assign memory at placement_address
        // and increment it by sz. Even when we've coded our kernel
        // heap, this will be useful for use before the heap is initialised.
        if (align == 1 && (placement_address & 0xFFFFF000) ) {
            // Align the placement address;
            placement_address &= 0xFFFFF000;
            placement_address += 0x1000;
        }
        if (phys) {
            *phys = placement_address;
        }
        uint32_t tmp = placement_address;
        placement_address += sz;
        qemu_printf("Allocated ");
        qemu_printf(itoa(sz));
        qemu_printf(" bytes of RAM, starting at: ");
        qemu_printf(itoa(tmp));
        qemu_printf("\n");
        return tmp;
    }

    uint32_t kmalloc_a(uint32_t sz) {
        return kmalloc_int(sz, 1, 0);
    }

    uint32_t kmalloc_p(uint32_t sz, uint32_t *phys) {
        return kmalloc_int(sz, 0, phys);
    }

    uint32_t kmalloc_ap(uint32_t sz, uint32_t *phys) {
        return kmalloc_int(sz, 1, phys);
    }

    uint32_t kmalloc(uint32_t sz) {
        return kmalloc_int(sz, 0, 0);
    }

}