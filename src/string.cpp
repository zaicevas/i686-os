#include <string.h>

uint32_t strlen(const char* s) {
    uint32_t len = 0;
    while(*s++)
        len++;
    return len;
}
