#include <fs.h>
#include <terminal.h>
#include <memory.h>
#include <string.h>
#include <system.h>
#include <debug.h>

using namespace terminal;
using namespace memory;

namespace fs {

    multiboot_module **files;
    static uint8_t files_count = 0;
    static uint8_t file_names_strlen = 0;
    static uint8_t current_file_index = 0;

    void init(uint8_t files_count) {
        fs::files_count = files_count;
        files = (multiboot_module**) kmalloc(sizeof(multiboot_module*) * files_count);
    }

    void add_file(multiboot_module *module) {
        files[current_file_index++] = module;
        char *end = (char *) module->mod_end;
        *(end-1) = 0;
        file_names_strlen += strlen(module->string);
    }

    char *ls() {
        char *result = (char *) kmalloc(file_names_strlen + files_count); // files_count for spaces
        uint8_t j = 0;
        for (uint8_t i=0; i<files_count; i++) {
            memcpy((uint8_t *) result + j, (uint8_t *) files[i]->string, strlen(files[i]->string));
            result[j + strlen(files[i]->string)] = ' ';
            j += strlen(files[i]->string) + 1;
        }
        result[j-1] = 0;
        return result;
    }

    char *cat(char *file_name) {
        multiboot_module *file = nullptr;
        for (uint8_t i=0; i<files_count; i++) {
            if (are_strings_equal(files[i]->string, file_name))
                file = files[i];
        }
        if (file == nullptr)
            return "err: no such file";
        
        return (char *) file->mod_start;
    }

}