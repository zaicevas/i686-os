#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

namespace process {

    struct process_t {
        uint16_t id;
        char *name = "";
        bool active = false;
    };

    uint16_t alive_process_count = 0;
    process_t alive_processes[32];

    void start(char *file_name);
    void kill(uint16_t id);

    void suspend(uint16_t id);
    void resume(uint16_t id);

}

#endif
