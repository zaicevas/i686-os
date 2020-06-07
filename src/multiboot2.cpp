#include <multiboot2.h>
#include <debug.h>
#include <string.h>

#define MULTIBOOT_HEADER_MAGIC 0xE85250D6
#define MULTIBOOT_HEADER_ARCHITECTURE 0		// 32-bit (protected) i386

#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN 6
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER 5
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_SIZE 20
#define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST 1
#define MULTIBOOT_HEADER_FLAGS_NOT_OPTIONAL 0

#define MULTIBOOT_MODULE_TAG_TYPE 3

#define MULTIBOOT_INFORMATION_TAG_TYPE_FRAMEBUFFER 8
#define MULTIBOOT_INFORMATION_TAG_TYPE_BASIC_MEMINFO 4
#define MULTIBOOT_INFORMATION_TAG_TYPE_MEMORY_MAP 6
#define MULTIBOOT_INFORMATION_TAG_TYPE_END 0

struct multiboot_tag {
  uint32_t type;
  uint32_t size;
};

// currently not used 
struct multiboot_vbe {
	uint32_t type;
	uint32_t size;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
};

struct multiboot_header_tag_module_align {
  uint16_t type = MULTIBOOT_HEADER_TAG_MODULE_ALIGN;
  uint16_t flags = MULTIBOOT_HEADER_FLAGS_NOT_OPTIONAL;
  uint32_t size = 8;
};

struct __attribute__((aligned (8))) multiboot_header_tag_framebuffer {
	const uint16_t type = MULTIBOOT_HEADER_TAG_FRAMEBUFFER;
	const uint16_t flags = MULTIBOOT_HEADER_FLAGS_NOT_OPTIONAL;
	const uint32_t size = MULTIBOOT_HEADER_TAG_FRAMEBUFFER_SIZE;
	const uint32_t width = 0;
	const uint32_t height = 0;
	const uint32_t depth = 32;
};

struct __attribute__((aligned (8))) multiboot_header_tag_information_request {
	const uint16_t type = MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST;
	const uint16_t flags = MULTIBOOT_HEADER_FLAGS_NOT_OPTIONAL;
	const uint32_t size = 16;

	// requested tags
	const uint32_t framebuffer = MULTIBOOT_INFORMATION_TAG_TYPE_FRAMEBUFFER;
};

// denotes last tag
struct multiboot_header_tag_terminator {
	const uint16_t type = 0;
	const uint16_t flags = 0;
	const uint32_t size = 8;
};

struct multiboot_header {
	const uint32_t magic = MULTIBOOT_HEADER_MAGIC;
	const uint32_t architecture = MULTIBOOT_HEADER_ARCHITECTURE;
	const uint32_t header_length = 72;
	const uint32_t checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCHITECTURE + header_length);

	multiboot_header_tag_framebuffer mb_tag_framebuffer;
	multiboot_header_tag_information_request mb_info_request;
	multiboot_header_tag_module_align mb_module_align;
	multiboot_header_tag_terminator terminator;
} mb_header __attribute__((section(".multiboot")));

void print_framebuffer_debug(multiboot_framebuffer *framebuffer) {
	qemu_printf("framebuffer_bpp: ");
	qemu_printf(itoa(framebuffer->framebuffer_bpp));
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

// TOOD: refactor to get all multiboot information structs in one loop
multiboot_basic_memory_information *get_basic_meminfo(uint64_t addr) {
	multiboot_basic_memory_information *meminfo = nullptr;	
	for (multiboot_tag *tag = (multiboot_tag*) (addr + 8);
        tag->type != MULTIBOOT_INFORMATION_TAG_TYPE_END;
        tag = (struct multiboot_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) {

		switch (tag->type) {
			case MULTIBOOT_INFORMATION_TAG_TYPE_BASIC_MEMINFO:
			{
				meminfo = (multiboot_basic_memory_information*) tag;
			}
				break;
		}
    }
    return meminfo; 
}

multiboot_tag_mmap *get_memory_map(uint64_t addr) {
    multiboot_tag_mmap *mmap = nullptr;
	for (multiboot_tag *tag = (multiboot_tag*) (addr + 8);
        tag->type != MULTIBOOT_INFORMATION_TAG_TYPE_END;
        tag = (struct multiboot_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) {

		switch (tag->type) {
			case MULTIBOOT_INFORMATION_TAG_TYPE_MEMORY_MAP:
			{
				mmap = (multiboot_tag_mmap*) tag;
			}
				break;
		}
    }
    return mmap; 
}

uint16_t get_modules_count(uint64_t addr) {
	uint16_t count = 0;
    multiboot_module *module = nullptr;
	for (multiboot_tag *tag = (multiboot_tag*) (addr + 8);
        tag->type != MULTIBOOT_INFORMATION_TAG_TYPE_END;
        tag = (struct multiboot_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) {
		switch (tag->type) {
			case MULTIBOOT_MODULE_TAG_TYPE:
			{
				module = (multiboot_module*) tag;
				qemu_printf("Module found, start: ");
				qemu_printf(itoa(module->mod_start));
				qemu_printf(", end: ");
				qemu_printf(itoa(module->mod_end));
				qemu_printf(", size: ");
				qemu_printf(itoa(module->size));
				qemu_printf(", string: ");
				qemu_printf(module->string);
				qemu_printf(".");
				qemu_printf("\n");
				count++;
			}
				break;
		}
    }
    return count; 
}

multiboot_module *get_module(uint64_t addr, uint16_t index) {
    multiboot_module *module = nullptr;
	uint16_t i = 0;
	for (multiboot_tag *tag = (multiboot_tag*) (addr + 8);
        tag->type != MULTIBOOT_INFORMATION_TAG_TYPE_END;
        tag = (struct multiboot_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) {
		switch (tag->type) {
			case MULTIBOOT_MODULE_TAG_TYPE:
			{
				module = (multiboot_module*) tag;
				if (i++ == index)
					return module;
			}
				break;
		}
    }
	return nullptr;
}

multiboot_framebuffer *get_framebuffer(uint64_t addr) {
    multiboot_framebuffer *framebuffer = nullptr;
	for (multiboot_tag *tag = (multiboot_tag*) (addr + 8);
        tag->type != MULTIBOOT_INFORMATION_TAG_TYPE_END;
        tag = (struct multiboot_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) {

		switch (tag->type) {
			case MULTIBOOT_INFORMATION_TAG_TYPE_FRAMEBUFFER:
			{
				framebuffer = (multiboot_framebuffer*) tag;
				print_framebuffer_debug(framebuffer);
			}
				break;
			case 7:
			{
				multiboot_vbe *vbe = (multiboot_vbe*) tag;
				qemu_printf("vbe mode: ");
				qemu_printf(itoa(vbe->vbe_mode));
				qemu_printf("\n");
			}
				break;
		}
    }
    return framebuffer; 
}
