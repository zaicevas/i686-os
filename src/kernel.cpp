#include <stdint.h>

//#define MULTIBOOT1_HEADER_MAGIC 0x1BADB002
//#define MULTIBOOT1_HEADER_FLAGS 0x00000003

#define MULTIBOOT_HEADER_MAGIC 0xE85250D6
#define MULTIBOOT_HEADER_ARCHITECTURE 0x0		// 32-bit (procteted) i386

extern char _binary_assets_Goha_16_ps;

struct multiboot_flag_terminator {
	uint16_t type = 0;
	uint16_t flags = 0;
	uint32_t size = 8;
} mb_flag_terminator;

struct multiboot_header {
	uint32_t magic = MULTIBOOT_HEADER_MAGIC;
	uint32_t architecture = MULTIBOOT_HEADER_ARCHITECTURE;
	uint32_t header_length = 24;
	uint32_t checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCHITECTURE + 24);
	multiboot_flag_terminator terminator;

} mb_header __attribute__((section(".multiboot")));

/*
struct multiboot1_header {
	uint32_t magic = MULTIBOOT1_HEADER_MAGIC;
	uint32_t flags = MULTIBOOT_HEADER_FLAGS;
	uint32_t checksum = -(MULTIBOOT1_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS);
} mb1_header __attribute__((section(".multiboot")));
*/


#if defined(__cplusplus)
extern "C"
#endif
int kernel_main() {

	return 0;
}
