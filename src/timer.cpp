#include <timer.h>
#include <pic.h>
#include <terminal.h>
#include <system.h>
#include <scheduler.h>
#include <debug.h>

namespace timer {

    static uint64_t ticks = 0; // should never overflow

	__attribute__((interrupt)) void timer(struct interrupt_frame *frame) {
		ticks++;
        scheduler::do_switch();
		END_OF_INTERRUPT 
    }

    void set_frequency(uint16_t hz) { // hz times in a second
        uint32_t divisor = 1193180 / hz;       /* Calculate our divisor */

        outb(0x43, 0x36);             /* Set our command byte 0x36 */
        outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
        outb(0x40, divisor >> 8);     /* Set high byte of divisor */
    }

    void init() {
		set_frequency(TIMER_HZ_FREQUENCY);

		pic::set_gate(0x20, (uint32_t) &timer);
		pic::unmask(0);
    }

    uint64_t get_ticks() {
        return ticks;
    }

}
