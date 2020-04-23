#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB 1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 2

#include <stdint.h>

struct multiboot_framebuffer_color_info {	  // only relevant when framebuffer_type = 1 (RBA)
	uint8_t framebuffer_red_field_position;
	uint8_t framebuffer_red_mask_size;
	uint8_t framebuffer_green_field_position;
	uint8_t framebuffer_green_mask_size;
	uint8_t framebuffer_blue_field_position;
	uint8_t framebuffer_blue_mask_size;
};

struct multiboot_framebuffer {
	uint32_t type;
	uint32_t size;
  	uint64_t framebuffer_addr;
  	uint32_t framebuffer_pitch;
  	uint32_t framebuffer_width;
  	uint32_t framebuffer_height;
  	uint8_t framebuffer_bpp;
  	uint8_t framebuffer_type;
  	uint16_t reserved;

	multiboot_framebuffer_color_info color_info;
};

struct multiboot_basic_memory_information {
	uint32_t type;
	uint32_t size;
	uint32_t mem_lower;
	uint32_t mem_upper;
};

multiboot_framebuffer *get_framebuffer(uint64_t addr);
multiboot_basic_memory_information *get_basic_meminfo(uint64_t addr);

#endif
