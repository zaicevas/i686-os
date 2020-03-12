#ifndef SYSTEM_H 
#define SYSTEM_H

#include <stdint.h>

uint8_t *memcpy(uint8_t *dest, const uint8_t *src, uint64_t count);

uint8_t *memset(uint8_t *dest, uint8_t val, uint64_t count);

uint16_t *memsetw(uint16_t *dest, uint16_t val, uint64_t count);

uint16_t inb(uint16_t port);

void outb(uint16_t port, uint8_t val);

void halt();

void reboot();

#endif
