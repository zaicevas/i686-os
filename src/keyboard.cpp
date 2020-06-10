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
#define PS2_EMPTY_BUFFER 0

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_IRQ 1

#define CAPS_LOCK 0x3A
#define ENABLE_CAPS_LOCK 0b00000100

namespace keyboard {

	static bool caps_lock_led = false;
	static bool caps_lock_released = true;
	static bool is_shift_pressed = false;

	static void putc(uint8_t key);
	static char to_capital_letter(char c); 
	static char with_shift(char c); 
	static void wait_for_empty_buffer(); 
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
		const uint8_t status = inb(KEYBOARD_STATUS_PORT);
		
		if (status & 0x01) {
			uint8_t key = inb(KEYBOARD_DATA_PORT);

			if (key == ENTER_PRESSED) {
				terminal::handle_enter();
			}
			else if (key == KEYCODE::CAPS_LOCK_PRESSED && caps_lock_released) {
				caps_lock_released = false;
				caps_lock_led = !caps_lock_led;
				switch_caps_lock_led();
			}
			else if (key == KEYCODE::CAPS_LOCK_RELEASED) 
				caps_lock_released = true;
			else if (key == KEYCODE::LEFT_SHIFT_PRESSED || key == KEYCODE::RIGHT_SHIFT_PRESSED || key == KEYCODE::LEFT_SHIFT_RELEASED || key == KEYCODE::RIGHT_SHIFT_RELEASED)
				is_shift_pressed = key == KEYCODE::LEFT_SHIFT_PRESSED || key == KEYCODE::RIGHT_SHIFT_PRESSED;
			else if (keyboard_to_ascii(key) != 0) 
				putc(key);

		}

		END_OF_INTERRUPT	
	}

	inline static void putc(uint8_t key) {
		char ascii = keyboard_to_ascii(key);
		bool is_letter = ascii >= 'a' && key <= 'z';

		char c = ascii;

		if (is_letter && (caps_lock_led || is_shift_pressed))
			c = to_capital_letter(ascii);
		else if (is_shift_pressed)
			c = with_shift(ascii);

		terminal::kputc(c);
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

	inline static void wait_for_empty_buffer() {
		while ((inb(PS2_COMMAND) & 2) != PS2_EMPTY_BUFFER) {}
	}

	inline static void switch_caps_lock_led() {
		uint8_t led = caps_lock_led ? ENABLE_CAPS_LOCK : 0;

		wait_for_empty_buffer();

		outb(PS2_DATA, 0xED);

		wait_for_empty_buffer();

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

