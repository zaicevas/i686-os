#include <string.h>

uint32_t strlen(const char* s) {
    uint32_t len = 0;
    while(*s++)
        len++;
    return len;
}

char* itoa(uint64_t val) {
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
