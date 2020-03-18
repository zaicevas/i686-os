#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define BACKSPACE_ASCII 8

namespace keyboard {
	void init();
	uint8_t keyboard_to_ascii(uint8_t key);
}

#endif
