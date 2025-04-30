#include <stdio.h>

#include <kernel/fb_tty.h>

void kernel_main(char* multiboot_struct) {
	framebuffer_initialize(multiboot_struct);
	fillrect(512, 320, 20, 20, FB_BLUE);
	fb_terminal_setup ();
	fb_writestring ("coucou,\nceci est le Veldos");
	fb_writeat ("blergh", 6, 512, 320, FB_RED);
}
