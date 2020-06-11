#include <scheduler.h>
#include <terminal.h>
#include <fs.h>
#include <multiboot2.h>
#include <debug.h>
#include <terminal.h>

const uint8_t MAX_PROCESS_COUNT = 10;

using namespace terminal;

namespace scheduler {
    uint8_t alive_process_count = 0;
    process_t processes[MAX_PROCESS_COUNT];
    uint8_t active_process_index = 0;
    bool is_shell_mode = true;

    void move_out_of_shell_mode();

    void init() {
        for (uint8_t i=0; i<MAX_PROCESS_COUNT; i++) {
            processes[i] = EMPTY_PROCESS;
        }
    }

    // corresponds to processes[id]
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

        uint8_t id = generate_process_id();

        process_t new_process = {
            id,
            file_name,
            file->mod_start,
            false
        };

        processes[id] = new_process;
        alive_process_count++;
    }

    void kill_process(uint8_t id) {
        processes[id].is_ended = true;
    }

    void on_keyboard_terminate_interrupt(uint8_t number) {
        kill_process(number);
    }

    void do_switch() {
        if (is_shell_mode && alive_process_count > 0) {
            move_out_of_shell_mode();
        }
        else if (is_shell_mode && alive_process_count == 0) {
            move_to_shell_mode();
        }
        if (alive_process_count >= 2) {
            qemu_printf("more than 2 processes, have to switch");
            qemu_printf("\n");
        }
    }

    void move_to_shell_mode() {
        is_shell_mode = true;
        terminal::enable_user_shell();
    }

    void move_out_of_shell_mode() {
        is_shell_mode = false;
        terminal::disable_user_shell();
    }

    void on_process_sys_exit() {
        kprintf("Process (id: %u) has successfully ended\n", active_process_index);
        kill_process(active_process_index);
    }

    bool get_is_shell_mode() {
        return is_shell_mode;
    }
}