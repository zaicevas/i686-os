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

#define PSF_FONT_MAGIC 0x864ab572
 
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

void qemu_print_font(PSF_font font) {
/*
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
*/
}

#if defined(__cplusplus)
extern "C"
#endif
int kernel_main(uint64_t addr) {
	multiboot2_info boot_info = get_multiboot2_info(addr);


	if (boot_info.framebuffer_type == FRAMEBUFFER_TYPE::RGB)
		draw_text(&boot_info.canvas, "Hello, graphics mode!");
	else {
		const char *str = "Hello, text mode!";
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
