#include <stdio.h>

#include <kernel/fb_tty.h>
#include <drivers/keyboard.h>
#include <games/morpion.h>
#include <shell/shell.h>

void kernel_main(char* multiboot_struct) {
	framebuffer_initialize(multiboot_struct);
	fb_terminal_setup ();
	fb_writestring ("coucou,\nceci est le Veldos\n");
        shell ();
        /*fb_writeat ("alergh", 512, 304, FB_RED);
	char entree;
        while(1)
          {
            entree = keyboard_read();
            if (entree == 14)
              {
                fb_removechar();
              }
            else
              {
                fb_putchar(scancode_to_char(entree));
              };
          };*/
        /*fb_writeat ("alergh", 512, 336, FB_RED);*/
	//morpion ();
}
