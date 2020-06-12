#include <timer.h>
#include <pic.h>
#include <terminal.h>
#include <system.h>
#include <scheduler.h>
#include <debug.h>

namespace timer {

    static uint64_t ticks = 0; // should never overflow

    typedef unsigned int uword_t __attribute__ ((mode (__word__)));
    struct interrupt_frame {
        uword_t ip;
        uword_t cs;
        uword_t flags;
        uword_t sp;
        uword_t ss;
    };

	__attribute__((interrupt)) void timer(interrupt_frame *frame) {
		ticks++;
		END_OF_INTERRUPT 
        scheduler::do_switch();
    }

    void set_frequency(uint16_t hz) { // hz times in a second
        uint32_t divisor = 1193180 / hz;       /* Calculate our divisor */

        outb(0x43, 0x36);             /* Set our command byte 0x36 */
        outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
        outb(0x40, divisor >> 8);     /* Set high byte of divisor */
    }

    void init() {
		set_frequency(TIMER_HZ_FREQUENCY);

		pic::set_gate(0x20, (uint32_t) &timer, true);
		pic::unmask(0);
    }

    uint64_t get_ticks() {
        return ticks;
    }

}
