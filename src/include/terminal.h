#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <multiboot2.h>
#include <stdarg.h>

#define BACKSPACE 8
#define TAB 9
#define TAB_LENGTH 8

namespace terminal { 
	enum VGA_MODE { TEXT, GRAPHICS, UNKNOWN };

	struct canvas_t {
		uint32_t width;
		uint32_t height;
		uint32_t bytes_per_pixel;
		uint32_t bytes_per_line;
		uint8_t *framebuffer_addr = nullptr;
	};

	extern uint8_t chars_x;
	extern uint8_t chars_y;
	extern canvas_t screen_canvas;

	void init(multiboot_framebuffer framebuffer);
	void kprintf(const char *s, ...);

	VGA_MODE get_vga_mode();
	canvas_t get_screen_canvas();

}

#endif
