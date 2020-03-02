#include <debug.h>
#include <stdint.h>
#include <stddef.h>

// https://wiki.osdev.org/Inline_Assembly/Examples

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port) );
}

void qemu_printf(char *s, ...) {
	for (char *val = s; *val; val++) {
		outb(PORT_COM1, *val);
	}
}
