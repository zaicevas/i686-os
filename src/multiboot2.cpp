#include <multiboot2.h>
#include <debug.h>
#include <string.h>

#define MULTIBOOT_HEADER_MAGIC 0xE85250D6
#define MULTIBOOT_HEADER_ARCHITECTURE 0		// 32-bit (protected) i386

#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER 5
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_SIZE 20
#define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST 1
#define MULTIBOOT_HEADER_FLAGS_NOT_OPTIONAL 0

#define MULTIBOOT_INFORMATION_TAG_TYPE_END 0
#define MULTIBOOT_INFORMATION_TAG_TYPE_FRAMEBUFFER 8

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

// denotes last tag
struct multiboot_tag_terminator {
	const uint16_t type = 0;
	const uint16_t flags = 0;
	const uint32_t size = 8;
};

struct multiboot_header {
	const uint32_t magic = MULTIBOOT_HEADER_MAGIC;
	const uint32_t architecture = MULTIBOOT_HEADER_ARCHITECTURE;
	const uint32_t header_length = 64;
	const uint32_t checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCHITECTURE + header_length);

	multiboot_tag_framebuffer mb_tag_framebuffer;
	multiboot_tag_information_request mb_info_request;
	multiboot_tag_terminator terminator;
} mb_header __attribute__((section(".multiboot")));

multiboot2_info get_multiboot2_info(uint64_t addr) {
    multiboot2_info result;
	for (multiboot_tag *tag = (multiboot_tag*) (addr + 8);
        tag->type != MULTIBOOT_INFORMATION_TAG_TYPE_END;
        tag = (struct multiboot_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) {

		switch (tag->type) {
			case MULTIBOOT_INFORMATION_TAG_TYPE_FRAMEBUFFER:
			{
				multiboot_tag_framebuffer_common *framebuffer = (multiboot_tag_framebuffer_common*) tag;

				result.canvas = { 
					framebuffer->framebuffer_width, 
					framebuffer->framebuffer_height, 
					(framebuffer->framebuffer_bpp + 7) / 8, 
					framebuffer->framebuffer_pitch,
					(void*) framebuffer->framebuffer_addr,
				};

                result.color_info = framebuffer->color_info;

                result.framebuffer_type = static_cast<FRAMEBUFFER_TYPE>(framebuffer->framebuffer_type);

				qemu_printf("framebuffer_bpp: ");
				qemu_printf(itoa( framebuffer->framebuffer_bpp));
				qemu_printf("\n");

				qemu_printf("framebuffer_addr: ");
				qemu_printf(itoa( framebuffer->framebuffer_addr));
				qemu_printf("\n");

				qemu_printf("framebuffer_type: ");
				qemu_printf(itoa( framebuffer->framebuffer_type));
				qemu_printf("\n");

				qemu_printf("width: ");
				qemu_printf(itoa( framebuffer->framebuffer_width ));
				qemu_printf("\n");

				qemu_printf("height: ");
				qemu_printf(itoa( framebuffer->framebuffer_height));
				qemu_printf("\n");

				qemu_printf("pitch: ");
				qemu_printf(itoa( framebuffer->framebuffer_pitch));
				qemu_printf("\n");

				qemu_printf("red_field_position: ");
				qemu_printf(itoa( (framebuffer->color_info).framebuffer_red_field_position));
				qemu_printf("\n");

				qemu_printf("green_field_position: ");
				qemu_printf(itoa( (framebuffer->color_info).framebuffer_green_field_position));
				qemu_printf("\n");

				qemu_printf("blue_field_position: ");
				qemu_printf(itoa( (framebuffer->color_info).framebuffer_blue_field_position));
				qemu_printf("\n");

				qemu_printf("red_mask_size: ");
				qemu_printf(itoa( (framebuffer->color_info).framebuffer_red_mask_size));
				qemu_printf("\n");
			}
				break;
		}
    }
    return result; 
}