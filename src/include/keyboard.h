#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define KEYBOARD_SCANCODES 128

namespace keyboard {
	void init();
	extern uint8_t keyboard_map[KEYBOARD_SCANCODES];
}

#endif
