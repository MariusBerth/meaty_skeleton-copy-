#include <stdio.h>

#include <kernel/fb_tty.h>

void kernel_main(char* multiboot_struct) {
	framebuffer_initialize(multiboot_struct);
	fillrect(0, 0, 20, 20, 255, 255, 255);
}
