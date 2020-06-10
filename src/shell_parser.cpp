#include <shell_parser.h>
#include <string.h>
#include <debug.h>

namespace shell_parser {
    static char *help_msg = "TomixOS, version 0.1 (i686-elf)\n \
        hey there! \
        sup";

    PARSED_COMMAND parse(char *input) {
        char *trimmed_input = trim(input);
        qemu_printf(trimmed_input);
        qemu_printf("\n");
        qemu_printf(input);
        qemu_printf("\n");
        if (are_strings_equal(trimmed_input, "?")) {
            return PARSED_COMMAND::HELP;
        }
        return PARSED_COMMAND::UNKNOWN;
    }

    char *build_kernel_output(char *input) {
        PARSED_COMMAND cmd = parse(input);
        if (cmd == PARSED_COMMAND::HELP)
            return help_msg;

        return "err: unknown command";
    }

}