#include <cat.h>
#include <terminal.h>

using namespace terminal;

void cat(uint8_t *file_addr) {
    kprintf("%s\n", file_addr);
}