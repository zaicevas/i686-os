#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <multiboot2.h>
#include <stdarg.h>

#define TAB_LENGTH 8

namespace terminal { 
	const char* const PS1 = "[root ~]$ ";

	enum VGA_MODE { TEXT, GRAPHICS, UNKNOWN };
	VGA_MODE get_vga_mode();

    struct color_scheme_t {
        uint8_t red_position;
        uint8_t green_position;
        uint8_t blue_position;
        uint8_t alpha_position;
    };

	struct canvas_t {
		uint32_t width;
		uint32_t height;
		uint32_t bytes_per_pixel;
		uint32_t bytes_per_line;
		uint8_t *framebuffer_addr = nullptr;
		color_scheme_t color_scheme;
	};

	void init(multiboot_framebuffer framebuffer);
	void kprintf(const char *s, ...) __attribute__ ((format (printf, 1, 2)));
	void kputc(char c);
	void handle_enter();
	void init_user_shell(); 
	void delete_char();

	canvas_t get_screen_canvas();

	void set_chars_x(uint16_t chars_x);
	void set_chars_y(uint16_t chars_y);

	uint16_t get_chars_x();
	uint16_t get_chars_y();

	void enable_user_shell();
	void disable_user_shell();
	bool is_user_shell_initialized();
}

#endif
