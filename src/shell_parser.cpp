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
        "file_name - run file if it's an executable\n"
        "file_name X - run file X times concurrently\n"
        "Ctrl+X - kill process with pid X\n"
        "read X - reads X from HDD\n"
        "delete X - deletes X from HDD\n"
        "write X __text__ - writes to file X in HDD. Everything after 'write ' is included\n";

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
        else if (contains_first_word(input, "call_interrupt") || contains_first_word(input, "call_interrupt_long")) {
            return PARSED_COMMAND::RUN_PROCESS;
        }
        else if (contains_first_word(input, "read")) {
            return PARSED_COMMAND::READ;
        }
        else if (contains_first_word(input, "write")) {
            return PARSED_COMMAND::WRITE;
        }
        else if (contains_first_word(input, "delete")) {
            return PARSED_COMMAND::DELETE;
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
        else if (cmd == PARSED_COMMAND::READ) {
            if (!get_second_word(input))
                return "err: wrong arguments";
            qemu_printf(get_second_word(input));
            uint16_t number = atoi(get_second_word(input));
            if (number >= 1024) {
                return "err: wrong arguments";
            }
            return fs::read_from_hdd(number);
            // return fs::read_from_hdd();
        }
        else if (cmd == PARSED_COMMAND::DELETE) {
            if (!get_second_word(input))
                return "err: wrong arguments";
            qemu_printf(get_second_word(input));
            uint16_t number = atoi(get_second_word(input));
            if (number >= 1024) {
                return "err: wrong arguments";
            }
            fs::clear_file(number);
            return "";
        }
        else if (cmd == PARSED_COMMAND::WRITE) {
            if (!ignore_first_word_include_spaces(input) || 
                !ignore_first_word_include_spaces(ignore_first_word_include_spaces(input))) {
                return "err: please specify what you want to write";
            }
            qemu_printf(get_second_word(input));
            uint16_t number = atoi(get_second_word(input));
            qemu_printf("number: ");
            qemu_printf(itoa(number));
            qemu_printf("\n");
            if (number >= 1024) {
                return "err: wrong arguments";
            }

            char *text = ignore_first_word_include_spaces(ignore_first_word_include_spaces(input));

            fs::write_to_hdd(number, text);
            return fs::read_from_hdd(number);
            // return nullptr;
            // return fs::read_from_hdd();
        }
        else if (cmd == PARSED_COMMAND::RUN_PROCESS) {
            qemu_printf(get_second_word(input));
            if (!get_second_word(input)) {
                scheduler::add_process(trim(input));
                return nullptr;
            }
            else {
                char *second_word = get_second_word(input);
                if (strlen(second_word) != 1 || second_word[0] > '9' || second_word[0] < 0) {
                    return "err: wrong arguments";
                }
                if ((second_word[0] -'0') == 0)
                    return "";
                for (uint8_t i=0; i<second_word[0] - '0'; i++) {
                    if (contains_first_word(input, "call_interrupt"))
                        scheduler::add_process("call_interrupt");
                    else if (contains_first_word(input, "call_interrupt_long"))
                        scheduler::add_process("call_interrupt_long");
                }
                return nullptr;
            }
        }

        return "err: unknown command";
    }

}