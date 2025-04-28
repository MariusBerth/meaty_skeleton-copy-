#include <stdio.h>

#include <kernel/fb_tty.h>

void kernel_main(char* multiboot_struct) {
	framebuffer_initialize(multiboot_struct);
	fillrect(512, 320, 20, 20, FB_BLUE);
	fb_putchar (0,0,FB_WHITE,'a');
}
