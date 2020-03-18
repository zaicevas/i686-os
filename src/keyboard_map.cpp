#include <keyboard.h>

namespace keyboard {
	static char *qwertzuiop = "qwertyuiop[]"; 
	static char *asdfghjkl = "asdfghjkl;'";
	static char *yxcvbnm = "zxcvbnm";
	static char *num = "1234567890-=";

	enum KEYCODE {
		NULL_KEY = 0,
		Q_PRESSED = 0x10,
		Q_RELEASED = 0x90,
		W_PRESSED = 0x11,
		W_RELEASED = 0x91,
		E_PRESSED = 0x12,
		E_RELEASED = 0x92,
		R_PRESSED = 0x13,
		R_RELEASED = 0x93,
		T_PRESSED = 0x14,
		T_RELEASED = 0x94,
		Z_PRESSED = 0x15,
		Z_RELEASED = 0x95,
		U_PRESSED = 0x16,
		U_RELEASED = 0x96,
		I_PRESSED = 0x17,
		I_RELEASED = 0x97,
		O_PRESSED = 0x18,
		O_RELEASED = 0x98,
		P_PRESSED = 0x19,
		P_RELEASED = 0x99,
		A_PRESSED = 0x1E,
		A_RELEASED = 0x9E,
		S_PRESSED = 0x1F,
		S_RELEASED = 0x9F,
		D_PRESSED = 0x20,
		D_RELEASED = 0xA0,
		F_PRESSED = 0x21,
		F_RELEASED = 0xA1,
		G_PRESSED = 0x22,
		G_RELEASED = 0xA2,
		H_PRESSED = 0x23,
		H_RELEASED = 0xA3,
		J_PRESSED = 0x24,
		J_RELEASED = 0xA4,
		K_PRESSED = 0x25,
		K_RELEASED = 0xA5,
		L_PRESSED = 0x26,
		L_RELEASED = 0xA6,
		Y_PRESSED = 0x2C,
		Y_RELEASED = 0xAC,
		X_PRESSED = 0x2D,
		X_RELEASED = 0xAD,
		C_PRESSED = 0x2E,
		C_RELEASED = 0xAE,
		V_PRESSED = 0x2F,
		V_RELEASED = 0xAF,
		B_PRESSED = 0x30,
		B_RELEASED = 0xB0,
		N_PRESSED = 0x31,
		N_RELEASED = 0xB1,
		M_PRESSED = 0x32,
		M_RELEASED = 0xB2,

		ZERO_PRESSED = 0x0B,
		ONE_PRESSED = 0x2,
		NINE_PRESSED = 0xA,

		POINT_PRESSED = 0x34,
		POINT_RELEASED = 0xB4,

		SLASH_PRESSED = 0x35,
		SLASH_RELEASED = 0xB5,

		BACKSPACE_PRESSED = 0xE,
		BACKSPACE_RELEASED = 0x8E,
		SPACE_PRESSED = 0x39,
		SPACE_RELEASED = 0xB9,
		ENTER_PRESSED = 0x1C,
		ENTER_RELEASED = 0x9C,

		COMMA_PRESSED = 0x33,
		COMMA_RELEASED = 0xB3,
		EQUALS_PRESSED = 0x0D,
		EQUALS_RELEASED = 0x8D,
		BACK_TICK_PRESSED = 0x29,
		BACK_TICK_RELEASED = 0xA9,
		BACKSLASH_PRESSED = 0x2B,
		BACKSLASH_RELEASED = 0xAB

	};

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