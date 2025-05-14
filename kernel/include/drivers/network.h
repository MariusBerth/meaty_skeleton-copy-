#include <stddef.h>

void card_setup (void);
int rtl_transmit (volatile void* packet, size_t length);
int rtl_poll (volatile void* DstBuf);
