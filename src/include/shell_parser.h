#ifndef SHELL_PARSER_H
#define SHELL_PARSER_H

#include <stdint.h>

namespace shell_parser {

    enum PARSED_COMMAND {
        UNKNOWN,
        LS,
        CAT,
        HELP,
        ECHO
    };

    PARSED_COMMAND parse(char *input);

}

#endif
