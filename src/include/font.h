#ifndef FONT_H

#define FONT_H
#include <stdint.h>

#define FNT_FONTHEIGHT 8
#define FNT_FONTWIDTH 8

#define VESA_COLOR_WHITE 0x00ffffff
#define VESA_COLOR_BLACK 0x00000000
#define VESA_COLOR_RED   0x00ff0000
#define VESA_COLOR_GREEN 0x0000ff00
#define VESA_COLOR_BLUE  0x000000ff
#define VESA_COLOR_MAGENTA  0x00ff00ff

struct pixel_t {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha = 1;
};

struct canvas_t {
	uint32_t width;
	uint32_t height;
	uint32_t bytes_per_pixel;
	uint32_t bytes_per_line;
	void *framebuffer_addr;
};

uint8_t *get_font_array();

uint32_t get_font_color();

void set_font_color(uint32_t color);

void draw_text(canvas_t *canvas, char *text);

#endif
