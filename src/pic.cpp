#include <pic.h>
#include <system.h>
#include <idt.h>
#include <string.h>
#include <debug.h>
#include <terminal.h>
#include <scheduler.h>

// other interrupts are disabled for this gate, while handler is being run
// read more: https://wiki.osdev.org/Interrupt_Descriptor_Table
#define INTERRUPT_GATE 0x8E
// 'cli' and 'sti' are not called for traps
#define TRAP_GATE 0x8F

#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_DATA 0xA1

using namespace idt;
using namespace terminal;

/*
0 - Division by zero exception
1 - Debug exception
2 - Non maskable interrupt
3 - Breakpoint exception
4 - 'Into detected overflow'
5 - Out of bounds exception
6 - Invalid opcode exception
7 - No coprocessor exception
8 - Double fault (pushes an error code)
9 - Coprocessor segment overrun
10 - Bad TSS (pushes an error code)
11 - Segment not present (pushes an error code)
12 - Stack fault (pushes an error code)
13 - General protection fault (pushes an error code)
14 - Page fault (pushes an error code)
15 - Unknown interrupt exception
16 - Coprocessor fault
17 - Alignment check exception (pushes an error code)
18 - Machine check exception
19-31 - Reserved
21 pushes an error code
*/

typedef unsigned int uword_t __attribute__ ((mode (__word__)));
struct interrupt_frame {
    uword_t ip;
    uword_t cs;
    uword_t flags;
    uword_t sp;
    uword_t ss;
};
typedef void (*isr_t)(uword_t);

// TODO: investigate bug, when 2 interrupt handlers have an exact same body
// and you get 'interrupt service routine can't be called directly' error
namespace pic {

	void setup_cpu_dedicated_irqs();
	void handle_cpu_interrupt(uint32_t);

	static uint16_t syscall_count = 0;

	uint32_t interrupt_handlers[256];

	void register_interrupt_handler(uint8_t index, uint32_t handler_address) {
		interrupt_handlers[index] = handler_address;
	}

	// https://forum.osdev.org/viewtopic.php?f=1&t=24218 discussion regarding division by zero infinite loop
	__attribute__((interrupt)) void isr0(interrupt_frame *frame) {
		kprintf("\nDivision by zero handled\n");
		// to reenable keyboard, uncomment:
		// asm volatile ("sti");
		halt();
		END_OF_INTERRUPT
	}

	__attribute__((interrupt)) void isr1(interrupt_frame *frame) {
		kprintf("\nDebug exception handled\n");
		END_OF_INTERRUPT
	}

	__attribute__((interrupt)) void isr2(interrupt_frame *frame) {
		kprintf("\nNon maskable interrupt handled\n");
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr3(interrupt_frame *frame) {
		kprintf("\nBreakpoint exception handled\n");
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr4(interrupt_frame *frame) {
		kprintf("\nInto detected overflow handled\n");
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr5(interrupt_frame *frame) {
		kprintf("\nOut of bounds exception handled\n");
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr6(interrupt_frame *frame) {
		kprintf("\nInvalid opcode exception handled\n");
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr7(interrupt_frame *frame) {
		kprintf("\nNo coprocessor exception handled\n");
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr8(interrupt_frame *frame, uword_t error_code) {
		kprintf("\nDouble fault handled, error code: %u\n", error_code);
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr9(interrupt_frame *frame) {
		kprintf("\nCoprocessor segment overrun handled\n");
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr10(interrupt_frame *frame, uword_t error_code) {
		kprintf("\nBad TSS handled, error code: %u\n", error_code);
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr11(interrupt_frame *frame, uword_t error_code) {
		kprintf("\nSegment no present handled, error code: %u\n", error_code);
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr12(interrupt_frame *frame, uword_t error_code) {
		kprintf("\nStack fault handled, error code: %u\n", error_code);
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr13(interrupt_frame *frame, uword_t error_code) {
		kprintf("\nGeneral protection fault handled, error code: %u\n", error_code);
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr14(interrupt_frame *frame, uword_t error_code) {
		if (interrupt_handlers[14] != 0) {
			isr_t call_handler = (isr_t) interrupt_handlers[14];
			call_handler(error_code);
		}
		kprintf("\nPage fault handled, error code: %u\n", error_code);
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr15(interrupt_frame *frame) {
		kprintf("\nUknown interrupt exception handled\n");
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr16(interrupt_frame *frame) {
		kprintf("\nCoprocessor fault handled\n");
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr17(interrupt_frame *frame, uword_t error_code) {
		kprintf("\nAlignment check exception handled\n");
		END_OF_INTERRUPT
	}
	__attribute__((interrupt)) void isr18(interrupt_frame *frame) {
		kprintf("\nMachine check exception handled\n");
		END_OF_INTERRUPT
	}

    void init() {
		/* ICW1 - begin initialization */
		outb(PIC1_COMMAND, 0x11);
		outb(0xA0 , 0x11);

		/* ICW2 - remap offset address of IDT */
		/*
		* In x86 protected mode, we have to remap the PICs beyond 0x20 because
		* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
		*/

		outb(PIC1_DATA, 0x20);
		outb(PIC2_DATA, 0x28);

		/* ICW3 - setup cascading */
		outb(PIC1_DATA, 0x00);
		outb(PIC2_DATA, 0x00);

		/* ICW4 - environment info */
		outb(PIC1_DATA, 0x01);
		outb(PIC2_DATA, 0x01);
		/* Initialization finished */

		/* mask interrupts */
		outb(PIC1_DATA, 0xFF);
		outb(PIC2_DATA, 0xFF);

		setup_cpu_dedicated_irqs();
		memset((uint8_t*) interrupt_handlers, 0, sizeof(uint32_t)*256);
    }

	__attribute__((interrupt)) void sys_call(interrupt_frame *frame) {
		uint32_t eax;
		asm volatile ( "mov %0, %%eax" : "=r"(eax) );
		kprintf("Hello from process with id: %u, eax: %u\n", scheduler::get_active_process_id(), eax);
		END_OF_INTERRUPT
	}

	__attribute__((interrupt)) void sys_exit(interrupt_frame *frame) {
		scheduler::on_process_sys_exit();
		END_OF_INTERRUPT
	}

	void setup_cpu_dedicated_irqs() {
		set_gate(0, (uint32_t) &isr0);
		set_gate(1, (uint32_t) &isr1);
		set_gate(2, (uint32_t) &isr2);
		set_gate(3, (uint32_t) &isr3);
		set_gate(4, (uint32_t) &isr4);
		set_gate(5, (uint32_t) &isr5);
		set_gate(6, (uint32_t) &isr6);
		set_gate(7, (uint32_t) &isr7);
		set_gate(8, (uint32_t) &isr8);
		set_gate(9, (uint32_t) &isr9);
		set_gate(10, (uint32_t) &isr10);
		set_gate(11, (uint32_t) &isr11);
		set_gate(12, (uint32_t) &isr12);
		set_gate(13, (uint32_t) &isr13);
		set_gate(14, (uint32_t) &isr14);
		set_gate(15, (uint32_t) &isr15);
		set_gate(16, (uint32_t) &isr16);
		set_gate(17, (uint32_t) &isr17);
		set_gate(18, (uint32_t) &isr18);
		set_gate(19, (uint32_t) &isr10);
		set_gate(20, (uint32_t) &isr11);
		set_gate(21, (uint32_t) &isr12);
		set_gate(22, (uint32_t) &isr13);
		set_gate(23, (uint32_t) &isr14);
		set_gate(24, (uint32_t) &isr15);
		set_gate(25, (uint32_t) &isr16);
		set_gate(26, (uint32_t) &isr17);
		set_gate(27, (uint32_t) &isr18);
		set_gate(28, (uint32_t) &isr18);
		set_gate(29, (uint32_t) &isr18);
		set_gate(30, (uint32_t) &isr18);
		set_gate(31, (uint32_t) &isr18);
		set_gate(0x80, (uint32_t) &sys_call);
		set_gate(0x81, (uint32_t) &sys_exit);
	}

	void set_gate(uint8_t index, uint32_t address) {
		IDT_entry *IDT = get_idt();

		IDT[index].offset_lowerbits = address & 0xFFFF;
		IDT[index].selector = KERNEL_CODE_SEGMENT_OFFSET;
		IDT[index].zero = 0;
		IDT[index].offset_higherbits = (address & 0xFFFF0000) >> 16;

		// We must uncomment the OR below when we get to using user-mode.
		// It sets the interrupt gate's privilege level to 3.
		IDT[index].type_attr = INTERRUPT_GATE /* | 0x60 */;
	}

	void set_gate(uint8_t index, uint32_t address, bool trap) {
		IDT_entry *IDT = get_idt();

		IDT[index].offset_lowerbits = address & 0xFFFF;
		IDT[index].selector = KERNEL_CODE_SEGMENT_OFFSET;
		IDT[index].zero = 0;
		IDT[index].offset_higherbits = (address & 0xFFFF0000) >> 16;

		IDT[index].type_attr = trap ? TRAP_GATE : INTERRUPT_GATE;
	}

	// enable interrupt
	void unmask(uint8_t irq) {
		uint16_t port = irq > 8 ? PIC2_DATA : PIC1_DATA;

		uint8_t imr = inb(port); // interrupt mask register
		uint8_t new_imr = imr ^ (1 << irq); 

		outb(port, new_imr);
	}

	// disable interrupt
	void mask(uint8_t irq) {
		uint16_t port = irq > 8 ? PIC2_DATA : PIC1_DATA;

		uint8_t imr = inb(port); // interrupt mask register
		uint8_t new_imr = imr | (1 << irq); 

		outb(port, new_imr);
	}

}
