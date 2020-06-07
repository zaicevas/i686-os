#include <system.h>
#include <debug.h>
#include <terminal.h>

#define KBRD_INTRFC 0x64
 
/* keyboard interface bits */
#define KBRD_BIT_KDATA 0 /* keyboard data is in buffer (output buffer is empty) (bit 0) */
#define KBRD_BIT_UDATA 1 /* user data is in buffer (command buffer is empty) (bit 1) */
 
#define KBRD_IO 0x60 /* keyboard IO port */
#define KBRD_RESET 0xFE /* reset CPU command */
 
#define bit(n) (1<<(n)) /* Set bit n to 1 */
 
/* Check if bit n in flags is set */
#define check_flag(flags, n) ((flags) & bit(n))

using namespace terminal;

void *memcpy(uint8_t *dest, uint8_t *src, uint64_t n) {
    uint8_t *p = dest;

    while (n--) {
        *dest++ = *src++;
    }

    return p;
}

void *memset(uint8_t *dest, uint8_t val, uint64_t n) {
    uint8_t *p = dest;
	while (n--)
		*dest++ = val;
	return p;
}

void *memsetw(uint16_t *dest, uint16_t val, uint64_t n) {
    uint16_t *p = dest;
	while (n--)
		*dest++ = val;
	return p;
}

uint8_t inb(uint16_t port) {
	uint16_t val;
	asm volatile ("inb %0, %1" : : "a"(val), "Nd"(port));
	return val;
}

uint16_t inw(uint16_t port) {
    uint16_t val;
	asm volatile ("inw %0, %1" : : "a"(val), "Nd"(port));
    return val;
}

void outb(uint16_t port, uint8_t val) {
	asm volatile ("outb %0, %1" : : "Nd"(port), "a"(val));
}

void outw(uint16_t port, uint16_t val) {
	asm volatile ("outw %0, %1" : : "Nd"(port), "a"(val));
}

bool are_interrupts_enabled() {
    uint32_t flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

void cpuid(uint16_t code, uint32_t *a, uint32_t *d) {
    asm volatile ( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx" );
}

void cpuGetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi) {
   asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}
 
void cpuSetMSR(uint32_t msr, uint32_t lo, uint32_t hi) {
   asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

uint32_t read_cr0() {
    uint32_t val;
    asm volatile ( "mov %0, %%cr0" : "=r"(val) );
    return val;
}

void write_cr0(uint32_t val) {
    asm volatile ( "mov %%cr0, %0" : : "a"(val) );
}

void halt() {
	loop:
		asm volatile ("hlt");
		goto loop;
}

void reboot() {
    uint8_t temp;
 
    asm volatile ("cli"); /* disable all interrupts */
 
    /* Clear all keyboard buffers (output and command buffers) */
    do
    {
        temp = inb(KBRD_INTRFC); /* empty user data */
        if (check_flag(temp, KBRD_BIT_KDATA) != 0)
            inb(KBRD_IO); /* empty keyboard data */
    } while (check_flag(temp, KBRD_BIT_UDATA) != 0);
 
    outb(KBRD_INTRFC, KBRD_RESET); /* pulse CPU reset line */

    halt();
}

void panic(const char *message, const char *file, uint32_t line) {
    asm volatile("cli"); 
    kprintf("PANIC(%s) at %s: %u\n", message, file, line);
    for(;;);
}
