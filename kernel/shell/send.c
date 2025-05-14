#include <drivers/network.h>
#include <string.h>

void send(char* str) {
	rtl_transmit (str, strlen(str) + 1);
}
			
