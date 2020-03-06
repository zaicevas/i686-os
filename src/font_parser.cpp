#include <font.h>
#include <stdint.h>
#include <string.h>
#include <debug.h>

#define RGB_DEPTH 24
#define VIDEO_MEMORY_ADDR 0xB8000

static uint8_t chars_x = 0;
static uint8_t chars_y = 0;

static const pixel_t WHITE = {0xff, 0xff, 0xff};
static const pixel_t BLACK = {0x00, 0x00, 0x00};

canvas_t screen_canvas = {};
static VGA_MODE vga_mode = VGA_MODE::TEXT;
static color_scheme_t graphics_color_scheme = { 2, 1, 0, 3 }; // BGRA by default

static void draw_pixel(canvas_t canvas, uint32_t x, uint32_t y, const pixel_t *pixel) {
	uint8_t *location = (uint8_t*) canvas.framebuffer_addr + (canvas.bytes_per_pixel * x) + (canvas.bytes_per_line * y);

	if (canvas.bytes_per_pixel > 3) {
		location[graphics_color_scheme.alpha_position] = pixel->alpha;
	}

	location[graphics_color_scheme.blue_position] = pixel->blue;
	location[graphics_color_scheme.green_position] = pixel->green;
	location[graphics_color_scheme.red_position] = pixel->red;
}

static void gpu_print_char(canvas_t canvas, char c) {
	const uint8_t *bmp = get_font(c);

	for(uint8_t width = 0; width < 8; width++) {
		for(uint8_t height = 0; height < 8; height++) {
		    uint8_t mask = 1 << width;
		    if (bmp[height] & mask)
				draw_pixel(canvas, chars_x * FONT_WIDTH + width, chars_y * FONT_HEIGHT + height, &WHITE);
		    else
				draw_pixel(canvas, chars_x * FONT_WIDTH + width, chars_y * FONT_HEIGHT + height, &BLACK);
		}
	}	

	chars_x++;

	uint32_t chars_per_line = canvas.bytes_per_line / (canvas.bytes_per_pixel * 8);

	if (chars_x >= chars_per_line) {
		chars_x = 0;
		chars_y++;
	}
}

static void gpu_print_text(canvas_t canvas, const char *text) {
	for (uint32_t i=0; i<strlen(text); i++)
		gpu_print_char(canvas, text[i]);
}

static void text_mode_clear_screen(canvas_t canvas) {
	uint8_t *video_memory = (uint8_t*) canvas.framebuffer_addr;
	for (uint16_t i=0; i<canvas.width * canvas.height * 2; i+=2) {
		video_memory[i] = ' ';
		video_memory[i+1] = 0x07; 		
	}
}

static void text_mode_print_text(canvas_t canvas, const char *text) {
	uint8_t *video_memory = (uint8_t*) canvas.framebuffer_addr;

	for (uint32_t i=0; i<strlen(text); i++) {
		uint16_t offset = chars_x * 2 + chars_y * canvas.bytes_per_line;
		video_memory[offset] = text[i];
		video_memory[offset+1] = 0x07;
		chars_x++;
	}
}

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

// returns which byte in the RGBA is meant for alpha
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

static color_scheme_t framebuffer_color_info_to_color_scheme(multiboot_framebuffer framebuffer) {
	multiboot_framebuffer_color_info color_info = framebuffer.color_info;

	return {
		color_info.framebuffer_red_field_position / 8,
		color_info.framebuffer_green_field_position / 8,
		color_info.framebuffer_blue_field_position / 8,
		framebuffer.framebuffer_bpp <= RGB_DEPTH ? -1 : get_alpha_byte_position(color_info) / 8
	};
}

void init_vga_mode(multiboot_framebuffer framebuffer) {
	screen_canvas = {
		framebuffer.framebuffer_width, 
		framebuffer.framebuffer_height, 
		(framebuffer.framebuffer_bpp + 7) / 8, 
		framebuffer.framebuffer_pitch,
		(uint8_t*) framebuffer.framebuffer_addr
	};

	graphics_color_scheme = framebuffer_color_info_to_color_scheme(framebuffer);
	vga_mode = framebuffer_type_to_VGA_MODE(framebuffer.framebuffer_type);

	if (vga_mode == VGA_MODE::TEXT)
		text_mode_clear_screen(screen_canvas);
}

void print_text(canvas_t canvas, const char *text) {
	if (vga_mode == VGA_MODE::GRAPHICS)
		gpu_print_text(canvas, text);
	else if (vga_mode == VGA_MODE::TEXT)
		text_mode_print_text(canvas, text);
}

void print_text(const char *text) {
	if (screen_canvas.framebuffer_addr == nullptr) {
		qemu_printf("ERROR: print_text(const char *text) was called with screen_canvas uninitialized");
		qemu_printf("\n");
		return;
	}
	
	print_text(screen_canvas, text);
}