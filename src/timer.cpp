#include <timer.h>
#include <pic.h>
#include <terminal.h>
#include <system.h>
#include <scheduler.h>
#include <debug.h>
#include <string.h>

namespace timer {

    static uint64_t ticks = 0; // should never overflow

    extern "C" void timer_interrupt_handler();

	extern "C" void call_scheduler(registers_t *registers) {
        // qemu_printf("eax: ");
        // qemu_printf(itoa(registers->eax));
        // qemu_printf(", ");
        // qemu_printf("ebx: ");
        // qemu_printf(itoa(registers->ebx));
        // qemu_printf(", ");
        // qemu_printf("ecx: ");
        // qemu_printf(itoa(registers->ecx));
        // qemu_printf(", ");
        // qemu_printf("edx: ");
        // qemu_printf(itoa(registers->edx));
        // qemu_printf(", ");
        // qemu_printf("edi: ");
        // qemu_printf(itoa(registers->edi));
        // qemu_printf(", ");
        // qemu_printf("esi: ");
        // qemu_printf(itoa(registers->esi));
        // qemu_printf(", ");
        // qemu_printf("cs: ");
        // qemu_printf(itoa(registers->cs));
        // qemu_printf(", ");
        // qemu_printf("eip: ");
        // qemu_printf(itoa(registers->eip));
        // qemu_printf(", ");
        // qemu_printf("esp: ");
        // qemu_printf(itoa(registers->esp));
        // qemu_printf(", ");
        // qemu_printf("ss: ");
        // qemu_printf(itoa(registers->ss));
        // qemu_printf(", ");
        // qemu_printf("eflags: ");
        // qemu_printf(itoa(registers->eflags));
        // qemu_printf(", ");
        // qemu_printf("iret_table_address: ");
        // qemu_printf(itoa(registers->iret_table_address));
        // qemu_printf(", ");
        // qemu_printf("&eip: ");
        // qemu_printf(itoa((uint32_t) &registers->eip));
        // qemu_printf(", ");
		ticks++;
        scheduler::do_switch(registers);
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

		pic::set_gate(0x20, (uint32_t) timer_interrupt_handler, true);
		pic::unmask(0);
    }

    uint64_t get_ticks() {
        return ticks;
    }

}
