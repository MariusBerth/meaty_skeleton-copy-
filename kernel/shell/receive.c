#include <drivers/network.h>
#include <kernel/fb_tty.h>

char buffer[1514];

void receive (void) {
	uint32_t length = rtl_poll(buffer);
	if (length) {
		fb_writestring (buffer);
	};
}
	

