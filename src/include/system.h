#ifndef SYSTEM_H 
#define SYSTEM_H

#include <stdint.h>

void *memcpy(uint8_t *dest, uint8_t *src, uint64_t n);

void *memset(uint8_t *dest, uint8_t val, uint64_t n);

void *memsetw(uint16_t *dest, uint16_t val, uint64_t n);

uint8_t inb(uint16_t port);

uint16_t in(uint16_t port);

void outb(uint16_t port, uint8_t val);

void out(uint16_t port, uint16_t val);

void halt();

void reboot();

#endif
