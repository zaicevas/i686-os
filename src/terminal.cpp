#include <stdint.h>
#include <string.h>
#include <debug.h>
#include <vga.h>
#include <gpu.h>
#include <terminal.h>

namespace terminal {

	static VGA_MODE vga_mode = VGA_MODE::UNKNOWN;

	static VGA_MODE framebuffer_type_to_VGA_MODE(uint8_t framebuffer_type) {
		switch (framebuffer_type) {
			case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
				return VGA_MODE::GRAPHICS;
				break;
			case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
				return VGA_MODE::TEXT;
				break;
			default:
				return VGA_MODE::UNKNOWN;
		}
	}

	void init(multiboot_framebuffer framebuffer) {
		screen_canvas = {
			framebuffer.framebuffer_width, 
			framebuffer.framebuffer_height, 
			(framebuffer.framebuffer_bpp + 7) / 8, 
			framebuffer.framebuffer_pitch,
			(uint8_t*) framebuffer.framebuffer_addr
		};

		vga_mode = framebuffer_type_to_VGA_MODE(framebuffer.framebuffer_type);

		if (vga_mode == VGA_MODE::TEXT)
			vga::init();
		else
			gpu::init(framebuffer);
	}

	void kprintf(const char *s, ...) {
		va_list args;
		va_start(args, s);

		if (vga_mode == VGA_MODE::GRAPHICS)
			gpu::kprintf(s, args);
		else if (vga_mode == VGA_MODE::TEXT)
			vga::kprint(s);

		va_end(args);
	}

	VGA_MODE get_vga_mode() {
		return vga_mode;
	}

	canvas_t get_screen_canvas() {
		return screen_canvas;
	}

	void set_chars_x(uint16_t chars_x) {
		terminal::chars_x = chars_x;
	}

	void set_chars_y(uint16_t chars_y) {
		terminal::chars_y = chars_y;
	}

	uint16_t get_chars_x() {
		return chars_x;
	}
	uint16_t get_chars_y() {
		return chars_y;
	}

}
