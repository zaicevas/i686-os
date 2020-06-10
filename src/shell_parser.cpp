#include <shell_parser.h>
#include <string.h>
#include <debug.h>
#include <system.h>

namespace shell_parser {
    static char *help_msg= ""
        "  __                  __        \n" 
        "_/  |_  ____   _____ |__|__  ___\n"
        "\\   __\\/  _ \\ /     \\|  \\  \\/  /\n"
        " |  | (  <_> )  Y Y  \\  |>    < \n"
        " |__|  \\____/|__|_|  /__/__/\\__\\\n\n"
        "TomixOS, version 0.1 (i686-elf)\n\n"
        "Shell commands:\n"
        "? - print this message\n"
        "reboot - reboot system\n"
        "ls - list directory contents\n"
        "cat file - print contents of file X\n"
        "echo X - print X\n"
        "ps - print all running processes\n";

    PARSED_COMMAND parse(char *input) {
        char *trimmed_input = trim(input);
        qemu_printf(trimmed_input);
        qemu_printf("\n");
        qemu_printf(input);
        qemu_printf("\n");
        if (are_strings_equal(trimmed_input, "?")) {
            return PARSED_COMMAND::HELP;
        }
        else if (are_strings_equal(trimmed_input, "reboot")) {
            return PARSED_COMMAND::REBOOT;
        }
        return PARSED_COMMAND::UNKNOWN;
    }

    char *build_kernel_output(char *input) {
        PARSED_COMMAND cmd = parse(input);
        if (cmd == PARSED_COMMAND::HELP)
            return help_msg;
        else if (cmd == PARSED_COMMAND::REBOOT) {
           reboot(); 
        }

        return "err: unknown command";
    }

}