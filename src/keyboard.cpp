#include <stdint.h>
#include <system.h>
#include <terminal.h>
#include <keyboard.h>
#include <string.h>
#include <debug.h>
#include <pic.h>

#define PS2_COMMAND 0x64
#define PS2_DATA 0x60
#define PS2_DISABLE_FIRST_PORT 0xAD
#define PS2_DISABLE_SECOND_PORT 0xA7
#define PS2_ENABLE_FIRST_PORT 0xAE
#define PS2_ENABLE_SECOND_PORT 0xA8
#define PS2_OUTPUT_FULL 0x1

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_IRQ 1

#define CAPS_LOCK 0x3A
#define ENABLE_CAPS_LOCK 0b00000100

namespace keyboard {

	static bool caps_lock = false;
	static bool is_shift_pressed = false;

	static void putc(uint8_t key);
	static char to_capital_letter(char c); 
	static char with_shift(char c); 
	static void ack();
	static void switch_caps_lock_led();
	static void disable_ps2();
	static void enable_ps2();
	static void flush_output_buffer();
	void keyboard_interrupt_handler(struct interrupt_frame *frame); 

	// https://wiki.osdev.org/%228042%22_PS/2_Controller#Initialising_the_PS.2F2_Controller
	void init() {
		disable_ps2();

		flush_output_buffer();

		enable_ps2();

		pic::set_gate(0x21, (uint32_t) &keyboard_interrupt_handler);
		pic::unmask(KEYBOARD_IRQ);
	}

	__attribute__((interrupt)) void keyboard_interrupt_handler(struct interrupt_frame *frame) {
		outb(0x20, 0x20);

		const uint8_t status = inb(KEYBOARD_STATUS_PORT);
		
		if (status & 0x01) {
			uint8_t key = inb(KEYBOARD_DATA_PORT);

			if (key == KEYCODE::CAPS_LOCK_PRESSED) {
				switch_caps_lock_led();
				return;
			}
			else if (key == KEYCODE::LEFT_SHIFT_PRESSED || key == KEYCODE::LEFT_SHIFT_RELEASED) {
				is_shift_pressed = key == KEYCODE::LEFT_SHIFT_PRESSED;
				return;
			}

			if (keyboard_to_ascii(key) != 0)
				putc(key);

		}
	}

	inline static void putc(uint8_t key) {
		char ascii = keyboard_to_ascii(key);
		bool is_letter = ascii >= 'a' && key <= 'z';

		char c = ascii;

		if (is_letter && (caps_lock || is_shift_pressed))
			c = to_capital_letter(ascii);
		else if (is_shift_pressed)
			c = with_shift(ascii);

		terminal::kprintf("%c", c);
	}

	inline static char with_shift(char c) {
		if (c == '/') return '?';
		else if (c == '.') return '>';
		else if (c == ',') return '<';
		else if (c == ']') return '}';
		else if (c == '[') return '{';
		else if (c == '\\') return '|';
		else if (c == '\'') return '"';
		else if (c == ';') return ':';
		else if (c >= '0' && c <= '9') return ")!@#$%^&*("[c - '0'];
		else if (c == '`') return '~';
		else if (c == '-') return '_';
		else if (c == '=') return '+';
		else return c;
	}

	inline static char to_capital_letter(char c) {
		return c - ('a' - 'A');
	}

	inline static void ack() {
		while (!(inb(0x60)==0xFA));
	}

	inline static void switch_caps_lock_led() {
		caps_lock = !caps_lock;

		uint8_t led = caps_lock ? ENABLE_CAPS_LOCK : 0;

		outb(0x60, 0xED);
		ack();
		outb(PS2_DATA, led);
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

}

