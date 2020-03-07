#include <system.h>

uint8_t *memcpy(uint8_t *dest, const uint8_t *src, uint64_t count) {
    return nullptr;
}

uint8_t *memset(uint8_t *dest, uint8_t val, uint64_t count) {
    return nullptr;
}

uint16_t *memsetw(uint16_t *dest, uint16_t val, uint64_t count) {
    return nullptr;
}

uint16_t inb(uint16_t port) {
    uint16_t val;
	asm volatile ("inb %0, %1" : : "Nd"(port), "a"(val) );
    return val;
}

void outb(uint16_t port, uint8_t val) {
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port) );
}