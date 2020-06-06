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

    void init(uint64_t multiboot_addr) {
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
}