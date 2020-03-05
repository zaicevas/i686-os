#include <debug.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// https://wiki.osdev.org/Inline_Assembly/Examples

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port) );
}

void qemu_printf(const char *s, ...) {
	for (uint32_t i=0; i<strlen(s); i++) {
		outb(PORT_COM1, s[i]);
	}
}
