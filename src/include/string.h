#ifndef STRING_H
#define STRING_H

#include <stdint.h>

uint32_t strlen(const char *s);

char *itoa(uint64_t val);

bool are_strings_equal(const char *lhs, const char *rhs);

bool contains_first_word(char *test, char *word);

char *get_second_word(char *str);

char *ignore_first_word(char *str);
char *ignore_first_word_include_spaces(char *str);

bool ends_by(char *test, char *end);

char *trim(const char *s);

#endif
