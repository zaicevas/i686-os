#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <multiboot2.h>

namespace fs {

    void init(uint8_t files_count);
    void add_file(multiboot_module *module);
    char *ls();
    char *cat(char *file_name);
    multiboot_module *get_file_by_name(char *name);

}

#endif
