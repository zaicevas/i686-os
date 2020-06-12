#ifndef SYSTEM_H 
#define SYSTEM_H

#define PANIC(msg) panic(msg, __FILE__, __LINE__);

#include <stdint.h>

// should be moved to memory.h
void *memcpy(uint8_t *dest, uint8_t *src, uint64_t n);

void *memset(uint8_t *dest, uint8_t val, uint64_t n);

void *memsetw(uint16_t *dest, uint16_t val, uint64_t n);

uint8_t inb(uint16_t port);

uint16_t inw(uint16_t port);

void outb(uint16_t port, uint8_t val);

void outw(uint16_t port, uint16_t val);

bool are_interrupts_enabled();

void cpuid(uint16_t code, uint32_t *a, uint32_t *d);

void cpuGetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi);
 
void cpuSetMSR(uint32_t msr, uint32_t lo, uint32_t hi);

uint32_t read_cr0();

void enable_interrupts();
void disable_interrupts();

void write_cr0(uint32_t val);

void halt();

void reboot();

void panic(const char *message, const char *file, uint32_t line);

#endif
