#include <string.h>
#include <memory.h>
#include <debug.h>

uint32_t strlen(const char *s) {
    uint32_t len = 0;
    while(*s++)
        len++;
    return len;
}

char *itoa(uint64_t val) {
// http://www.strudel.org.uk/itoa/
	if (val == 0)
		return "0";
	static char buf[32] = {0};
	const uint8_t BASE = 10;

	uint64_t i = 30;
	
	for(; val && i ; --i, val /= BASE)
		buf[i] = "0123456789abcdef"[val % BASE];
	
	return &buf[i+1];
}

bool are_strings_equal(const char *lhs, const char *rhs) {
	uint32_t strlen_lhs = strlen(lhs);
	uint32_t strlen_rhs = strlen(rhs);
	if (strlen_lhs != strlen_rhs)
		return false;
	for (uint32_t i=0; i<strlen_lhs; i++) {
		if (lhs[i] != rhs[i])
			return false;
	}
	return true;
}

bool is_white_space(char c) {
	return c <= ' ' || c >= 0x7f;
}

// returns true, if first word of 'test' is 'word'
// e.g. contains_first_word('cat a.txt', 'cat') => true
bool contains_first_word(char *test, char *word) {
	uint32_t test_length = strlen(test);
	uint32_t word_length = strlen(word);
	uint32_t match_length = 0;
	uint32_t first_non_white_space_index = 0;
	for (uint32_t i=0; i<test_length; i++) {
		if (!is_white_space(test[i])) {
			first_non_white_space_index = i;
			break;
		}
	}

	if (test_length - first_non_white_space_index + 1 <= word_length)
		return false;

	for (uint32_t i=0; i<word_length; i++) {
		if (word[i] == test[i + first_non_white_space_index])
			match_length++;
	}
	return (match_length == word_length && is_white_space(test[first_non_white_space_index + word_length]));
}

char *get_second_word(char *str) {
	qemu_printf("get_second_word(");
	qemu_printf(str);
	qemu_printf(")\n");
	uint32_t str_length = strlen(str);
	uint32_t second_word_index = 0;
	bool skipped_first_word = false;
	for (uint32_t i=1; i<str_length; i++) {
		if (!skipped_first_word && is_white_space(str[i]) && !is_white_space(str[i-1])) {
			skipped_first_word = true;
			qemu_printf("skipped_first_word when i: ");
			qemu_printf(itoa(i));
			qemu_printf("\n");
		}
		else if (skipped_first_word && !is_white_space(str[i])) {
			qemu_printf("second_word_index: ");
			qemu_printf(itoa(i));
			qemu_printf("\n");
			second_word_index = i;
			break;
		}
	}
	qemu_printf("second word_index: ");
	qemu_printf(itoa(second_word_index));
	qemu_printf(".\n");
	if (!skipped_first_word)
		return nullptr;
	char *second_word = (char *) memory::kmalloc(str_length - second_word_index + 1);
	for (uint32_t i=second_word_index; i<str_length; i++) {
		if (is_white_space(str[i])) {
			qemu_printf("zero when i: ");
			qemu_printf(itoa(i));
			qemu_printf("\n");
			second_word[i-second_word_index] = 0;
			break;
		}
		second_word[i-second_word_index] = str[i];
		if (i == str_length - 1)
			second_word[i-second_word_index+1] = 0;
	}
	qemu_printf("second word: ");
	qemu_printf(second_word);
	qemu_printf(".\n");
	return second_word;
}

char *trim(const char *s) {
	uint32_t length = strlen(s);
	char *trimmed = (char *) memory::kmalloc(length);
	uint32_t j = 0;
	for (uint32_t i=0; i<strlen(s); i++) {
		if (!is_white_space(s[i]))
			trimmed[j++] = s[i];
	}
	trimmed[j] = 0;
	return trimmed;
}
