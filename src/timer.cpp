#include <timer.h>
#include <pic.h>
#include <terminal.h>
#include <system.h>

namespace timer {
    struct interrupt_frame;
    uint64_t i = 0;

	__attribute__((interrupt)) void print_smth(interrupt_frame *frame) {
		outb(0x20, 0x20);
        //terminal::kprintf("%d ", i++);
    }

    void set_frequency(uint16_t hz) { // hz times in a second
        uint32_t divisor = 1193180 / hz;       /* Calculate our divisor */
        outb(0x43, 0x36);             /* Set our command byte 0x36 */
        outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
        outb(0x40, divisor >> 8);     /* Set high byte of divisor */
    }

    void init() {
        set_frequency(HZ_FREQUENCY);
		pic::set_gate(0x20, (uint32_t) &print_smth);
		pic::unmask(0);
    }
}
