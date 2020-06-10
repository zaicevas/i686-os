#include <string.h>
#include <memory.h>

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

static bool is_white_space(char c) {
	return c <= ' ' || c >= 0x7f;
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