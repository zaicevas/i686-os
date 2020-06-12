#include <keyboard.h>

namespace keyboard {

	static char *qwertzuiop = "qwertyuiop[]"; 
	static char *asdfghjkl = "asdfghjkl;'";
	static char *yxcvbnm = "zxcvbnm";
	static char *num = "1234567890-=";

	bool is_number(uint8_t key) {
		return key >= ONE_PRESSED && key <= EQUALS_PRESSED;
	}

	uint8_t keyboard_to_ascii(uint8_t key) {
		if (key == BACKSPACE_PRESSED) return '\b';
		if (key == ENTER_PRESSED) return '\n';
		if (key == SPACE_PRESSED) return ' ';
		if (key == POINT_PRESSED) return '.';
		if (key == COMMA_PRESSED) return ',';
		if (key == SLASH_PRESSED) return '/';
		if (key == BACK_TICK_PRESSED) return '`';
		if (key == BACKSLASH_PRESSED) return '\\';

		if (key >= ONE_PRESSED && key <= EQUALS_PRESSED)
			return num[key - ONE_PRESSED];

		if (key >= 0x10 && key <= 0x1C)
			return qwertzuiop[key - 0x10];
		else if (key >= 0x1E && key <= 0x28)
			return asdfghjkl[key - 0x1E];
		else if (key >= 0x2C && key <= 0x32)
			return yxcvbnm[key - 0x2C];

		return 0;
	}

}
