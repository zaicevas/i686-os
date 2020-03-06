/*#include <font.h>
#include <stdint.h>
#include <string.h>
#include <debug.h>

#define RGB_DEPTH 24
#define VIDEO_MEMORY_ADDR 0xB8000

static uint8_t chars_x = 0;
static uint8_t chars_y = 0;

static const pixel_t WHITE = {0xff, 0xff, 0xff};
static const pixel_t BLACK = {0x00, 0x00, 0x00};

canvas_t *screen_canvas;

static VGA_MODE vga_mode = VGA_MODE::TEXT;
static color_scheme_t graphics_color_scheme = { 2, 1, 0, 3 }; // BGRA by default
void init_vga_mode(multiboot_framebuffer framebuffer) {
	canvas_t canvas = {
		framebuffer.framebuffer_addr
	};
	screen_canvas->framebuffer_addr = framebuffer.framebuffer_addr;
	//screen_canvas = &canvas;
	qemu_printf("init_vga_mode: ");
	qemu_printf(itoa((uint64_t) screen_canvas->framebuffer_addr));
	qemu_printf("\n");
}

void print_text(canvas_t *canvas, const char *text) {
	qemu_printf("print_text2 with framebuffer_addr: ");
	qemu_printf(itoa((uint64_t) screen_canvas->framebuffer_addr));
	qemu_printf("\n");
}

void print_text(const char *text) {

	qemu_printf("print_text(char *text) with framebuffer addr: ");
	qemu_printf(itoa((uint64_t) screen_canvas->framebuffer_addr));
	qemu_printf("\n");
	
	print_text(screen_canvas, text);
}
*/