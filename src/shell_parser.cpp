#include <shell_parser.h>
#include <string.h>
#include <debug.h>

namespace shell_parser {
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
}