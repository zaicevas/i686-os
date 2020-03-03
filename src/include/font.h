#ifndef FONT_H
#define FONT_H

#include <stdint.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 8

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

uint8_t *get_font(char c);

void draw_text(canvas_t* canvas, char* text);

#endif
