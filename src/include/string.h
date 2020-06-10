#ifndef STRING_H
#define STRING_H

#include <stdint.h>

uint32_t strlen(const char *s);

char *itoa(uint64_t val);

bool are_strings_equal(const char *lhs, const char *rhs);

char *trim(const char *s);

#endif
