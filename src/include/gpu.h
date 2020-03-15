#ifndef GPU_H
#define GPU_H 

#include <stdint.h>
#include <multiboot2.h>
#include <stdarg.h>

#define FONT_WIDTH 10
#define FONT_HEIGHT 20

namespace gpu {

	struct pixel_t {
		uint8_t red;
		uint8_t green;
		uint8_t blue;
		uint8_t alpha;
	};

    	uint8_t *get_font(char c);

    	void init(multiboot_framebuffer framebuffer);
	void kprintf(const char *s, va_list args); 
    	void kputc(char c);
	void clear();

}

#endif
