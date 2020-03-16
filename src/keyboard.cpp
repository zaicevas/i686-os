#include <stdint.h>
#include <system.h>

#define PS2_COMMAND 0x64
#define PS2_DISABLE_FIRST_PORT 0xAD
#define PS2_DISABLE_SECOND_PORT 0xA7
#define PS2_ENABLE_FIRST_PORT 0xAE
#define PS2_ENABLE_SECOND_PORT 0xA8
#define PS2_OUTPUT_FULL 0x1
#define PS2_DATA 0x60

#define ENABLE_IRQ1 0b11111101

namespace keyboard {

	inline static void disable_ps2() {
		outb(PS2_COMMAND, PS2_DISABLE_FIRST_PORT);
		//outb(PS2_COMMAND, PS2_DISABLE_SECOND_PORT); // ignored if not supported
	}

	inline static void enable_ps2() {
		outb(PS2_COMMAND, PS2_ENABLE_FIRST_PORT);
		//outb(PS2_COMMAND, PS2_ENABLE_SECOND_PORT); /* ignored if unsupported? */
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

		outb(0x21 , ENABLE_IRQ1);
	}

}


