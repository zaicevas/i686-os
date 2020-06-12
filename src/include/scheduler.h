#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

namespace scheduler {

    struct process_t {
        uint8_t id;
        char *name;
        uint32_t entry_address;
        bool is_ended;
        bool has_started;
    };

    static process_t EMPTY_PROCESS = {
        0, "", 0, true, false
    };

    void init();

    void do_switch();
    void move_to_shell_mode();

    uint8_t generate_process_id();

    uint8_t add_process(char *file_name);
    // void start(char *file_name);
    void kill_process(uint8_t id);

    // will fire when alt+number is pressed
    void on_keyboard_terminate_interrupt(uint8_t number);

    void on_process_sys_exit();

    bool get_is_shell_mode();

}

#endif
