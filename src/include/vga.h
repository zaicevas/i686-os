#ifndef VGA_H 
#define VGA_H

#include <terminal.h>

namespace vga {

	void init(terminal::canvas_t screen_canvas);
	void clear();
	void kprint(const char *s);

}

#endif