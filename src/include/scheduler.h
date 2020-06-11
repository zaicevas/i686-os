#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <process.h>

using namespace process;

namespace scheduler {

    struct process_t {
        uint16_t id;
        char *name = "";
        bool active = false;
    };

    uint16_t alive_process_count = 0;
    process_t alive_processes[32];

    void do_switch();
    void move_to_shell_mode();

    void add_and_start_process(char *file_name);
    void start(char *file_name);
    void kill(uint16_t id);

    // will fire when alt+number is pressed
    void on_keyboard_terminate_interrupt(uint8_t number);

    void on_process_sys_exit();

}

#endif
