#include <debug.h>
#include <stdint.h>
#include <string.h>
#include <system.h>

#define PORT_COM1 0x3f8

void qemu_printf(const char *s, ...) {
	for (uint32_t i=0; i<strlen(s); i++) {
		outb(PORT_COM1, s[i]);
	}
}
