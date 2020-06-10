#include <shell_parser.h>
#include <string.h>

namespace shell_parser {
    PARSED_COMMAND parse(char *input) {
        char *trimmed_input = trim(input);
        if (are_strings_equal(trimmed_input, "?")) {
            return PARSED_COMMAND::HELP;
        }
        return PARSED_COMMAND::UNKNOWN;
    }
}