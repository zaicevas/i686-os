#include <stdint.h>

#if defined(__linux__)
	#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
	#error "This code must be compiled with an x86-elf compiler"
#endif

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

#if defined(__cplusplus)
extern "C"
#endif
int kernel_main() {
	return 0;
}
