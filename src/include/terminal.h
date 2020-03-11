#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <multiboot2.h>

#define BACKSPACE 0x08
#define TAB 0x0
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

	void print_text(canvas_t *canvas, const char *text);
	void print_text(const char *text);

	VGA_MODE get_vga_mode();

}

#endif
