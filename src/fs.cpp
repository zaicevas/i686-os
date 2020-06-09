#include <fs.h>
#include <terminal.h>
#include <memory.h>

using namespace terminal;
using namespace memory;

namespace fs {

    multiboot_module **files;
    static uint8_t files_count = 0;
    static uint8_t current_file_index = 0;

    void init(uint8_t files_count) {
        fs::files_count = files_count;
        files = (multiboot_module**) kmalloc(sizeof(multiboot_module*) * files_count);
    }

    void add_file(multiboot_module *module) {
        files[current_file_index++] = module;
    }

    void ls() {
        for (uint8_t i=0; i<files_count; i++)
            kprintf("%s ", files[i]->string);
        kprintf("\n");
    }

}