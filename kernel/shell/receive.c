#include <drivers/network.h>
#include <stdint.h>
#include <stdio.h>

char buffer[1514];

void receive (void) {
	uint32_t length = rtl_poll(buffer);
	if (length) {
		printf ("%s\n",buffer);
	};
}
	

