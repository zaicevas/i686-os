#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <multiboot2.h>

namespace fs {

    void init(uint8_t files_count);
    void init_hdd();
    void add_file(multiboot_module *module);
    char *ls();
    char *cat(char *file_name);
    multiboot_module *get_file_by_name(char *name);
    bool exists_executable(char *name);

    uint32_t get_furthest_file_address();

    void clear_file(uint16_t index);
    void write_to_hdd(uint16_t index, char *text);
    char *read_from_hdd(uint16_t index);
    // void write_to_hdd(char *text);
    // char *read_from_hdd();

}

#endif
