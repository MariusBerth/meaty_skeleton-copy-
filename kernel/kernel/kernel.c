#include <stdio.h>

#include <kernel/fb_tty.h>
#include <drivers/keyboard.h>

void kernel_main(char* multiboot_struct) {
	framebuffer_initialize(multiboot_struct);
	fillrect(512, 320, 20, 20, FB_BLUE);
	fb_terminal_setup ();
	fb_writestring ("coucou,\nceci est le Veldos");
        fb_writeat ("alergh", 6, 512, 304, FB_RED);
	while(1)
          {
            fb_putchar(keyboard_read());
            
          };
        fb_writeat ("alergh", 6, 512, 336, FB_RED);

}
