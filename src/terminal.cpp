#include <stdint.h>
#include <string.h>
#include <debug.h>
#include <vga.h>
#include <gpu.h>
#include <terminal.h>

namespace terminal {

	static VGA_MODE vga_mode = VGA_MODE::TEXT;

	uint8_t chars_x = 0;
	uint8_t chars_y = 0;

	canvas_t screen_canvas = {};

	static VGA_MODE framebuffer_type_to_VGA_MODE(uint8_t framebuffer_type) {
		switch (framebuffer_type) {
			case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
				return VGA_MODE::GRAPHICS;
				break;
			case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
				return VGA_MODE::TEXT;
				break;
			default:
				return VGA_MODE::TEXT;
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

	void print_text(canvas_t canvas, const char *text) {
		if (vga_mode == VGA_MODE::GRAPHICS)
			gpu::printf(text);
		else if (vga_mode == VGA_MODE::TEXT)
			vga::printf(text);
	}

	void print_text(const char *text) {
		if (screen_canvas.framebuffer_addr == nullptr) {
			qemu_printf("ERROR: print_text(const char *text) was called with screen_canvas uninitialized");
			qemu_printf("\n");
			return;
		}
		
		print_text(screen_canvas, text);
	}

}