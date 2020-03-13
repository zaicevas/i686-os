#include <stdint.h>
#include <system.h>

namespace keyboard {

	void init() {
		/* enable IRQ1 */
		outb(0x21 , 0b11111101);
	}

}
