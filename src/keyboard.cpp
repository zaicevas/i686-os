#include <stdint.h>
#include <system.h>

/*
void move_cursor(uint16_t x, uint16_t y) {
    uint16_t pos = y * 160 + x;

    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, pos);
}
*/
// osdev
void move_cursor(uint16_t x, uint16_t y) {
	uint16_t pos = y * 160 + x;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}