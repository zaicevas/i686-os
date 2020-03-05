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
