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
    uint16_t hdd_text_indexes[1024] = {0};

    char **hdd_texts = nullptr;

    void init(uint8_t files_count) {
        fs::files_count = files_count;
        files = (multiboot_module**) kmalloc(sizeof(multiboot_module*) * files_count);
    }

    void init_hdd() {
        hdd_texts = (char **) kmalloc(1024 * sizeof(char*));
        for (uint16_t i=0; i<1024; i++) {
            hdd_texts[i] = (char *) kmalloc(1024);
            memset((uint8_t *) hdd_texts[i], 0, 1024);
        }
    }

    multiboot_module *get_file_by_name(char *name) {
        for (uint8_t i=0; i<files_count; i++) {
            if (are_strings_equal(files[i]->string, name))
                return files[i];
        }
        return nullptr;
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

    bool exists_executable(char *name) {
        for (uint8_t i=0; i<files_count; i++) {
            if (are_strings_equal(files[i]->string, name) && !ends_by(name, ".txt")) {
                return true;
            }
        }
        return false;
    }

    void clear_file(uint16_t index) {
        memset((uint8_t *) hdd_texts[index], 0, 1024);
        hdd_text_indexes[index] = 0;
    }

    void write_to_hdd(uint16_t index, char *text) {
        for (uint16_t i=0; i<strlen(text); i++) {
            hdd_texts[index][hdd_text_indexes[index]++] = text[i];
        }
    }

    char *read_from_hdd(uint16_t index) {
        return hdd_texts[index];
    }

    uint32_t get_furthest_file_address() {
        uint32_t largest_address = 0;
        for (uint8_t i=0; i<files_count; i++) {
            if (files[i]->mod_end> largest_address) {
                largest_address = files[i]->mod_end;
            }
        }
        return largest_address;
    }

    // void write_to_hdd(char *text) {
    //     for (uint16_t i=0; i<strlen(text); i++) {
    //         hdd_text[hdd_text_index++] = text[i];
    //     }
    // }

    // char *read_from_hdd() {
    //     return hdd_text;
    // }

}