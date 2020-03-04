#include <stdint.h>
#include <stddef.h>
#include <font.h>
#include <debug.h>
#include <string.h>
#include <multiboot2.h>

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

/*
struct multiboot_framebuffer_color_info {	// only when framebuffer_type = 1
	uint8_t framebuffer_red_field_position;
	uint8_t framebuffer_red_mask_size;
	uint8_t framebuffer_green_field_position;
	uint8_t framebuffer_green_mask_size;
	uint8_t framebuffer_blue_field_position;
	uint8_t framebuffer_blue_mask_size;
}; */
 
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

// struct multiboot_tag {
//   uint32_t type;
//   uint32_t size;
// };

// struct multiboot_tag_vbe {
// 	uint32_t type;
// 	uint32_t size;
// 	uint16_t vbe_mode;
// 	uint16_t vbe_interface_seg;
// 	uint16_t vbe_interface_off;
// 	uint16_t vbe_interface_len;
// };

// struct multiboot_tag_framebuffer_common {
// 	uint32_t type;
// 	uint32_t size;
//   	uint64_t framebuffer_addr;
//   	uint32_t framebuffer_pitch;
//   	uint32_t framebuffer_width;
//   	uint32_t framebuffer_height;
//   	uint8_t framebuffer_bpp;
// #define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
// #define MULTIBOOT_FRAMEBUFFER_TYPE_RGB     1
// #define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT	2
//   	uint8_t framebuffer_type;
//   	uint16_t reserved;
// 	multiboot_framebuffer_color_info color_info;
// };

// struct __attribute__((aligned (8))) multiboot_tag_framebuffer {
// 	const uint16_t type = MULTIBOOT_HEADER_TAG_FRAMEBUFFER;
// 	const uint16_t flags = MULTIBOOT_HEADER_FLAGS_NOT_OPTIONAL;
// 	const uint32_t size = MULTIBOOT_HEADER_TAG_FRAMEBUFFER_SIZE;
// 	const uint32_t width = 1024;
// 	const uint32_t height = 768;
// 	const uint32_t depth = 32;
// };

// struct __attribute__((aligned (8))) multiboot_tag_information_request {
// 	const uint16_t type = MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST;
// 	const uint16_t flags = MULTIBOOT_HEADER_FLAGS_NOT_OPTIONAL;
// 	const uint32_t size = 16;

// 	// requested tags
// 	const uint32_t framebuffer = MULTIBOOT_INFORMATION_TAG_TYPE_FRAMEBUFFER;
// };

// struct multiboot_tag_terminator {
// 	const uint16_t type = 0;
// 	const uint16_t flags = 0;
// 	const uint32_t size = 8;
// };

// struct multiboot_header {
// 	const uint32_t magic = MULTIBOOT_HEADER_MAGIC;
// 	const uint32_t architecture = 0;
// 	const uint32_t header_length = 64;
// 	const uint32_t checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCHITECTURE + header_length);
// 	multiboot_tag_framebuffer mb_tag_framebuffer;
// 	multiboot_tag_information_request mb_info_request;
// 	multiboot_tag_terminator terminator;
// } mb_header __attribute__((section(".multiboot")));



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
	multiboot2_info boot_info = get_multiboot2_info(addr);

	if (boot_info.framebuffer_type == FRAMEBUFFER_TYPE::RGB)
		draw_text(&boot_info.canvas, "Hello, world!");
	else {
		char *str = "Hello, world!";
		char *vidptr = (char*) boot_info.canvas.framebuffer_addr;
		uint8_t i = 0, j = 0;
		while (str[j] != '\0') {
			vidptr[i] = str[j];
			vidptr[i+1] = 0x07;
			j++;
			i += 2;
		}
	}

	return 0;
}
