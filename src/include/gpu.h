#ifndef GPU_H
#define GPU_H 

#include <stdint.h>
#include <multiboot2.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

namespace gpu {

	struct pixel_t {
		uint8_t red;
		uint8_t green;
		uint8_t blue;
		uint8_t alpha;
	};

    uint8_t *get_font(char c);

    void init(multiboot_framebuffer framebuffer);
	void printf(const char *text);
	void clear();

}

#endif
