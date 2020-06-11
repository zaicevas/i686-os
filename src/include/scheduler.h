#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <process.h>

using namespace process;

namespace scheduler {

    uint16_t alive_process_count = 0;
    process_t alive_processes[32];

    void do_switch();
    void move_to_shell_mode();

}

#endif
