#include <system.h>

#define KBRD_INTRFC 0x64
 
/* keyboard interface bits */
#define KBRD_BIT_KDATA 0 /* keyboard data is in buffer (output buffer is empty) (bit 0) */
#define KBRD_BIT_UDATA 1 /* user data is in buffer (command buffer is empty) (bit 1) */
 
#define KBRD_IO 0x60 /* keyboard IO port */
#define KBRD_RESET 0xFE /* reset CPU command */
 
#define bit(n) (1<<(n)) /* Set bit n to 1 */
 
/* Check if bit n in flags is set */
#define check_flag(flags, n) ((flags) & bit(n))

void *memcpy(uint8_t *dest, const uint8_t *src, uint64_t n) {
    uint8_t *p = dest;
    while (n--)
        *dest++ = *src++;
    return p;
}

void *memset(uint8_t *dest, uint8_t val, uint64_t n) {
    uint8_t *p = dest;
	while (n--)
		*dest++ = val;
	return p;
}

void *memsetw(uint16_t *dest, uint16_t val, uint64_t n) {
    return nullptr;
}

uint16_t inb(uint16_t port) {
	uint16_t val;
	asm volatile ("in %1, %0" : : "Nd"(port), "a"(val) );
	return val;
}

void outb(uint16_t port, uint8_t val) {
	asm volatile ("out %1, %0" : : "a"(val), "Nd"(port) );
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
