#ifndef FONT_H 
#define FONT_H 

#include <stdint.h>

namespace font {
	uint8_t *get_bitmap(char c);
	uint8_t get_width(); 
	uint8_t get_height(); 
}

#endif
