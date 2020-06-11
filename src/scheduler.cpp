#include <scheduler.h>
#include <terminal.h>
#include <fs.h>
#include <multiboot2.h>

const uint8_t MAX_PROCESS_COUNT = 10;

namespace scheduler {
    uint16_t alive_process_count = 0;
    process_t processes[MAX_PROCESS_COUNT];
    uint8_t active_process_index = 0;
    bool is_shell_mode = true;


    void init() {
        for (uint8_t i=0; i<MAX_PROCESS_COUNT; i++) {
            processes[i] = EMPTY_PROCESS;
        }
    }

    uint8_t generate_process_id() {
        for (uint8_t i=0; i<MAX_PROCESS_COUNT; i++) {
            if (processes[i].is_ended)
                return i;
        }
        return 254; // if we get there, something went terribly wrong
    }

    void add_process(char *file_name) {
        multiboot_module *file = fs::get_file_by_name(file_name);
        if (!file)
            return;

        process_t new_process = {
            alive_process_count,
            file_name,
            file->mod_start,
            false
        };

        alive_process_count++;
    }

    void do_switch() {

    }

    void move_to_shell_mode() {
        is_shell_mode = true;
        terminal::enable_user_shell();
    }
}