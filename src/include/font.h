#ifndef FONT_H
#define FONT_H

#include <stdint.h>
#include <multiboot2.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

extern uint8_t chars_x;
extern uint8_t chars_y;

enum VGA_MODE { TEXT, GRAPHICS };

struct pixel_t {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha = 0;
};

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
};

extern canvas_t screen_canvas;

uint8_t *get_font(char c);

void init_vga_mode(multiboot_framebuffer framebuffer);

void print_text(canvas_t *canvas, const char *text);
void print_text(const char *text);

#endif
