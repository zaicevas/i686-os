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

    void init() {
		pic::set_gate(0x20, (uint32_t) &print_smth);
		pic::unmask(0);
    }
}
