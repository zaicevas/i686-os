#ifndef GPU_H
#define GPU_H 

#include <stdint.h>
#include <multiboot2.h>
#include <stdarg.h>
#include <terminal.h>

namespace gpu {

	struct pixel_t {
		uint8_t red;
		uint8_t green;
		uint8_t blue;
		uint8_t alpha;
	};

	void init(terminal::canvas_t screen_canvas);
	void kprintf(const char *s, va_list args); 
	void kputc(char c);
	void clear();

}

#endif
