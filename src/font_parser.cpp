#include <font.h>
#include <string.h>
#include <debug.h>
#include <stdint.h>
#include <string.h>

static uint8_t chars_x = 0;
static uint8_t chars_y = 0;

static const pixel_t WHITE = {0xff, 0xff, 0xff};
static const pixel_t BLACK = {0x00, 0x00, 0x00};

void draw_pixel(canvas_t *canvas, uint32_t x, uint32_t y, const pixel_t *pixel) {
	uint8_t *location = (uint8_t*) canvas->framebuffer_addr + (canvas->bytes_per_pixel * x) + (canvas->bytes_per_line * y);
	location[0] = pixel->blue;
	location[1] = pixel->green;
	location[2] = pixel->red;
	location[3] = pixel->alpha;
}

void draw_char(canvas_t *canvas, char c) {
	const uint8_t *bmp = get_font(c);

	for(uint8_t width = 0; width < 8; width++) {
		for(uint8_t height = 0; height < 8; height++) {
		    uint8_t mask = 1 << width;
		    if (bmp[height] & mask)
				draw_pixel(canvas, chars_x * FONT_WIDTH + width, chars_y * FONT_HEIGHT + height, &WHITE);
		    else
				draw_pixel(canvas, chars_x * FONT_WIDTH + width, chars_y * FONT_HEIGHT + height, &BLACK);
		}
	}	

	chars_x++;

	uint32_t chars_per_line = canvas->bytes_per_line / (canvas->bytes_per_pixel * 8);

	if (chars_x >= chars_per_line) {
		chars_x = 0;
		chars_y++;
	}
}

void draw_text(canvas_t *canvas, const char *text) {
	for (uint32_t i=0; i<strlen(text); i++)
		draw_char(canvas, text[i]);
}
