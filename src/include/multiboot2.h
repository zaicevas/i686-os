#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <stdint.h>
#include <font.h>

struct multiboot_framebuffer_color_info {	    // only when framebuffer_type = 1 (RBA)
	uint8_t framebuffer_red_field_position;
	uint8_t framebuffer_red_mask_size;
	uint8_t framebuffer_green_field_position;
	uint8_t framebuffer_green_mask_size;
	uint8_t framebuffer_blue_field_position;
	uint8_t framebuffer_blue_mask_size;
};

enum FRAMEBUFFER_TYPE { INDEXED, RGB, EGA_TEXT };

struct multiboot2_info {
    canvas_t canvas;
    FRAMEBUFFER_TYPE framebuffer_type;
    multiboot_framebuffer_color_info color_info;

    // more data will probably be needed in later stages
};

multiboot2_info get_multiboot2_info(uint64_t addr);

#endif