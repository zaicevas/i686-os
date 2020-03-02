#include <stdint.h>
#include <stddef.h>
#include <font.h>
#include <debug.h>

#if defined(__linux__)
	#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
	#error "This code must be compiled with an x86-elf compiler"
#endif

#define MULTIBOOT_HEADER_MAGIC 0xE85250D6
#define MULTIBOOT_HEADER_ARCHITECTURE 0x0		// 32-bit (protected) i386
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER 5
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_SIZE 20
#define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST 1
#define MULTIBOOT_HEADER_FLAGS_NOT_OPTIONAL 0

#define MULTIBOOT_INFORMATION_TAG_TYPE_END 0
#define MULTIBOOT_INFORMATION_TAG_TYPE_FRAMEBUFFER 8

#define PSF_FONT_MAGIC 0x864ab572

struct multiboot_framebuffer_color_info {	// only when framebuffer_type = 1
	uint8_t framebuffer_red_field_position;
	uint8_t framebuffer_red_mask_size;
	uint8_t framebuffer_green_field_position;
	uint8_t framebuffer_green_mask_size;
	uint8_t framebuffer_blue_field_position;
	uint8_t framebuffer_blue_mask_size;
};
 
struct PSF_font {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
};

PSF_font mem_font = {
	PSF_FONT_MAGIC,
	0,
	32,
	0,
	512,
	8,
	8,
	8
};

extern char _binary_assets_zap_vga16_psf_start;

struct multiboot_tag {
  uint32_t type;
  uint32_t size;
};

struct multiboot_tag_vbe {
	uint32_t type;
	uint32_t size;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
};

struct multiboot_tag_framebuffer_common {
	uint32_t type;
	uint32_t size;
  	uint64_t framebuffer_addr;
  	uint32_t framebuffer_pitch;
  	uint32_t framebuffer_width;
  	uint32_t framebuffer_height;
  	uint8_t framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB     1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT	2
  	uint8_t framebuffer_type;
  	uint16_t reserved;
	multiboot_framebuffer_color_info color_info;
};

struct __attribute__((aligned (8))) multiboot_tag_framebuffer {
	const uint16_t type = MULTIBOOT_HEADER_TAG_FRAMEBUFFER;
	const uint16_t flags = MULTIBOOT_HEADER_FLAGS_NOT_OPTIONAL;
	const uint32_t size = MULTIBOOT_HEADER_TAG_FRAMEBUFFER_SIZE;
	const uint32_t width = 1024;
	const uint32_t height = 768;
	const uint32_t depth = 32;
};

struct __attribute__((aligned (8))) multiboot_tag_information_request {
	const uint16_t type = MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST;
	const uint16_t flags = MULTIBOOT_HEADER_FLAGS_NOT_OPTIONAL;
	const uint32_t size = 16;

	// requested tags
	const uint32_t framebuffer = MULTIBOOT_INFORMATION_TAG_TYPE_FRAMEBUFFER;
};

struct multiboot_tag_terminator {
	const uint16_t type = 0;
	const uint16_t flags = 0;
	const uint32_t size = 8;
};

struct multiboot_header {
	const uint32_t magic = MULTIBOOT_HEADER_MAGIC;
	const uint32_t architecture = 0;
	const uint32_t header_length = 64;
	const uint32_t checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCHITECTURE + header_length);
	multiboot_tag_framebuffer mb_tag_framebuffer;
	multiboot_tag_information_request mb_info_request;
	multiboot_tag_terminator terminator;
} mb_header __attribute__((section(".multiboot")));

char* itoa(uint32_t val){
// http://www.strudel.org.uk/itoa/
	if (val == 0)
		return "0";
	static char buf[32] = {0};
	const uint64_t BASE = 10;
	
	uint64_t i = 30;
	
	for(; val && i ; --i, val /= BASE)
	
		buf[i] = "0123456789abcdef"[val % BASE];
	
	return &buf[i+1];
	
}

void qemu_print_font(PSF_font font) {
	qemu_printf("magic: ");
	qemu_printf(itoa(font.magic));	
	qemu_printf("\n");

	qemu_printf("bytesperglyph: ");
	qemu_printf(itoa(font.bytesperglyph));	
	qemu_printf("\n");

	qemu_printf("height: ");
	qemu_printf(itoa(font.height));	
	qemu_printf("\n");

	qemu_printf("width: ");
	qemu_printf(itoa(font.width));	
	qemu_printf("\n");
}

#if defined(__cplusplus)
extern "C"
#endif
int kernel_main(uint64_t addr) {
	uint32_t size = *(uint32_t *) addr;
	multiboot_tag *tag;
	void *framebuffer;
	canvas_t canvas;
	
	for (tag = (multiboot_tag*) (addr + 8); tag->type != MULTIBOOT_INFORMATION_TAG_TYPE_END; tag = (struct multiboot_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) {
		qemu_printf("tag: ");
		qemu_printf(itoa(tag->type));
		qemu_printf("\n");
		switch (tag->type) {
			case MULTIBOOT_INFORMATION_TAG_TYPE_FRAMEBUFFER:
			{
				multiboot_tag_framebuffer_common *fb = (multiboot_tag_framebuffer_common*) tag;
				framebuffer = (void*) fb->framebuffer_addr;
				
				canvas = { 
					1024, 
					768, 
					(fb->framebuffer_bpp + 7) / 8, 
					fb->framebuffer_pitch,
					(void*) fb->framebuffer_addr,
				};
				
				qemu_printf("framebuffer pointer: ");
				qemu_printf(itoa( (uint32_t) framebuffer));
				qemu_printf("\n");

				qemu_printf("framebuffer_bpp: ");
				qemu_printf(itoa( fb->framebuffer_bpp));
				qemu_printf("\n");

				qemu_printf("framebuffer_addr: ");
				qemu_printf(itoa( fb->framebuffer_addr));
				qemu_printf("\n");

				qemu_printf("framebuffer_type: ");
				qemu_printf(itoa( fb->framebuffer_type));
				qemu_printf("\n");

				qemu_printf("width: ");
				qemu_printf(itoa( fb->framebuffer_width ));
				qemu_printf("\n");

				qemu_printf("height: ");
				qemu_printf(itoa( fb->framebuffer_height));
				qemu_printf("\n");

				qemu_printf("pitch: ");
				qemu_printf(itoa( fb->framebuffer_pitch));
				qemu_printf("\n");

				qemu_printf("red_field_position: ");
				qemu_printf(itoa( (fb->color_info).framebuffer_red_field_position));
				qemu_printf("\n");

				qemu_printf("green_field_position: ");
				qemu_printf(itoa( (fb->color_info).framebuffer_green_field_position));
				qemu_printf("\n");

				qemu_printf("blue_field_position: ");
				qemu_printf(itoa( (fb->color_info).framebuffer_blue_field_position));
				qemu_printf("\n");

				qemu_printf("red_mask_size: ");
				qemu_printf(itoa( (fb->color_info).framebuffer_red_mask_size));
				qemu_printf("\n");
			}
				break;
			case 7:
			{
				multiboot_tag_vbe *vbe = (multiboot_tag_vbe*) tag;
				qemu_printf(itoa(vbe->vbe_mode));
				qemu_printf("\n");
			}
			break;
		}
	}

	uint8_t *font = get_font_array();

//	putchar((char *) framebuffer, 'h', 5, 10, 0x125347, 0x000000);

//	for (uint8_t i=0; i<100; i++) {
//		putchar('H', i, 350, 0xFFFFFF, 0x012345);
//	}

	char *yext = "Hello world!";
	draw_text(&canvas, yext);

	return 0;
}
