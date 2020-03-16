#include <stdint.h>
#include <system.h>
#include <terminal.h>
#include <idt.h>
#include <keyboard.h>
#include <string.h>
#include <debug.h>

#define PS2_COMMAND 0x64
#define PS2_DISABLE_FIRST_PORT 0xAD
#define PS2_DISABLE_SECOND_PORT 0xA7
#define PS2_ENABLE_FIRST_PORT 0xAE
#define PS2_ENABLE_SECOND_PORT 0xA8
#define PS2_OUTPUT_FULL 0x1
#define PS2_DATA 0x60

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

#define CAPS_LOCK 0x3A

#define ENABLE_IRQ1 0b11111101

namespace keyboard {

	static void kbd_ack() {
		while (!(inb(0x60)==0xFA));
	}

	static void kbd_led_handling(uint8_t ledstatus) {
		outb(0x60, 0xED);

		kbd_ack();

		outb(0x60, ledstatus);
	}

	struct interrupt_frame;

	bool caps_lock_led = false;

	__attribute__((interrupt)) void keyboard_interrupt_handler(interrupt_frame *frame) {
		outb(0x20, 0x20);

		const uint8_t status = inb(KEYBOARD_STATUS_PORT);
		
		if (status & 0x01) {
			uint8_t key = inb(KEYBOARD_DATA_PORT);

			if (key == CAPS_LOCK) {
				caps_lock_led = !caps_lock_led;
				kbd_led_handling(caps_lock_led ? 0b00000100 : 0);
				return;
			}

			if ((key < 0 || key >= 54) && key != 57)
				return;

			terminal::kprintf("%c", keyboard_map[key]);
		}
	}

	inline static void disable_ps2() {
		outb(PS2_COMMAND, PS2_DISABLE_FIRST_PORT);
	}

	inline static void enable_ps2() {
		outb(PS2_COMMAND, PS2_ENABLE_FIRST_PORT);
	}

	inline static void flush_output_buffer() {
		while (inb(PS2_COMMAND) & PS2_OUTPUT_FULL) {
			inb(PS2_DATA);
		}
	}

	// https://wiki.osdev.org/%228042%22_PS/2_Controller#Initialising_the_PS.2F2_Controller
	void init() {
		disable_ps2();

		flush_output_buffer();

		enable_ps2();

		idt::set_gate(0x21, (uint32_t) &keyboard_interrupt_handler);

		outb(0x21 , ENABLE_IRQ1);
	}

}


