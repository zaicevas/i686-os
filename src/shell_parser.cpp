#include <shell_parser.h>
#include <string.h>
#include <debug.h>
#include <system.h>
#include <fs.h>
#include <scheduler.h>

namespace shell_parser {
    static char *help_msg = 
        "  __                  __        \n" 
        "_/  |_  ____   _____ |__|__  ___\n"
        "\\   __\\/  _ \\ /     \\|  \\  \\/  /\n"
        " |  | (  <_> )  Y Y  \\  |>    < \n"
        " |__|  \\____/|__|_|  /__/__/\\__\\\n\n"
        "TomixOS, version 0.1 (i686-elf)\n\n"
        "Shell commands:\n"
        "? - print this message\n"
        "reboot - reboot system\n"
        "ls - list files\n"
        "cat file_name - print contents of file X\n"
        "echo X - print X\n"
        "file_name - run file if it's an executable\n";

    PARSED_COMMAND parse(char *input) {
        char *trimmed_input = trim(input);
        if (are_strings_equal(trimmed_input, "?")) {
            return PARSED_COMMAND::HELP;
        }
        else if (are_strings_equal(trimmed_input, "reboot")) {
            return PARSED_COMMAND::REBOOT;
        }
        else if (are_strings_equal(trimmed_input, "ls")) {
            return PARSED_COMMAND::LS;
        }
        else if (contains_first_word(input, "cat")) {
            return PARSED_COMMAND::CAT;
        }
        else if (contains_first_word(input, "echo")) {
            return PARSED_COMMAND::ECHO;
        }
        else if (fs::exists_executable(trimmed_input)) {
            return PARSED_COMMAND::RUN_PROCESS;
        }
        return PARSED_COMMAND::UNKNOWN;
    }

    char *build_kernel_output(char *input) {
        PARSED_COMMAND cmd = parse(input);
        if (cmd == PARSED_COMMAND::REBOOT) {
           reboot(); 
        }
        if (cmd == PARSED_COMMAND::HELP)
            return help_msg;
        else if (cmd == PARSED_COMMAND::LS) {
            return fs::ls();
        }
        else if (cmd == PARSED_COMMAND::CAT) {
            return fs::cat(get_second_word(input));
        }
        else if (cmd == PARSED_COMMAND::ECHO) {
            return ignore_first_word(input);
        }
        else if (cmd == PARSED_COMMAND::RUN_PROCESS) {
            scheduler::add_process(trim(input));
            scheduler::add_process(trim(input));
            return nullptr;
        }

        return "err: unknown command";
    }

}