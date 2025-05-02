#include <stdio.h>

#include <kernel/fb_tty.h>
#include <drivers/keyboard.h>
#include <games/morpion.h>

void kernel_main(char* multiboot_struct) {
	framebuffer_initialize(multiboot_struct);
	fb_terminal_setup ();
	/*fb_writestring ("coucou,\nceci est le Veldos");
        fb_writeat ("alergh", 512, 304, FB_RED);
	while(1)
          {
            fb_putchar(scancode_to_char(keyboard_read()));
            
          };
        fb_writeat ("alergh", 512, 336, FB_RED);*/
	morpion ();
}
