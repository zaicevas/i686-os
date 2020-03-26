#include <stdint.h>
#include <string.h>
#include <debug.h>
#include <vga.h>
#include <gpu.h>
#include <terminal.h>

#define RGB_DEPTH 24
namespace terminal {

	static VGA_MODE vga_mode = VGA_MODE::UNKNOWN;

	static uint8_t get_alpha_byte_position(multiboot_framebuffer_color_info color_info);
	static color_scheme_t framebuffer_color_info_to_color_scheme(multiboot_framebuffer framebuffer); 
	static VGA_MODE framebuffer_type_to_VGA_MODE(uint8_t framebuffer_type); 
	void init(multiboot_framebuffer framebuffer); 
	void kprintf(const char *s, ...); 


	void init(multiboot_framebuffer framebuffer) {
		screen_canvas = {
			framebuffer.framebuffer_width, 
			framebuffer.framebuffer_height, 
			(framebuffer.framebuffer_bpp + 7) / 8, 
			framebuffer.framebuffer_pitch,
			(uint8_t*) framebuffer.framebuffer_addr,
			framebuffer_color_info_to_color_scheme(framebuffer)
		};

		vga_mode = framebuffer_type_to_VGA_MODE(framebuffer.framebuffer_type);

		if (vga_mode == VGA_MODE::TEXT)
			vga::init();
		else
			gpu::init(screen_canvas);
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

    static uint8_t get_alpha_byte_position(multiboot_framebuffer_color_info color_info) {
        const uint8_t positions[] = {0, 8, 16, 24};

        const uint8_t given_positions[] = {
            color_info.framebuffer_blue_field_position, 
            color_info.framebuffer_green_field_position, 
            color_info.framebuffer_red_field_position
        };

        for (uint8_t i=0; i<4; i++) {
            bool exist = false;
            for (uint8_t j=0; j<3; j++) {
                if (positions[i] == given_positions[j])
                    exist = true;	
            }

            if (!exist)
                return positions[i] / 8;
        }

    }

    inline static color_scheme_t framebuffer_color_info_to_color_scheme(multiboot_framebuffer framebuffer) {
        multiboot_framebuffer_color_info color_info = framebuffer.color_info;

        return {
            color_info.framebuffer_red_field_position / 8,
            color_info.framebuffer_green_field_position / 8,
            color_info.framebuffer_blue_field_position / 8,
            framebuffer.framebuffer_bpp <= RGB_DEPTH ? -1 : get_alpha_byte_position(color_info) / 8
        };
    }

	inline static VGA_MODE framebuffer_type_to_VGA_MODE(uint8_t framebuffer_type) {
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
