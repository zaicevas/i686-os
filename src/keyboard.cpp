#include <stdint.h>
#include <system.h>

extern "C"
void write_port(unsigned short port, unsigned char data);

namespace keyboard {

	void init() {
		/* enable IRQ1 */
		//outb(0x21 , 0b11111101);
		//outb(0x21, 0xFD);
		write_port(0x21 , 0xFD);
	}

}
