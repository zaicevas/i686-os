#include <scheduler.h>
#include <terminal.h>

namespace scheduler {
    void do_switch() {

    }

    void move_to_shell_mode() {
        terminal::enable_user_shell();
    }
}