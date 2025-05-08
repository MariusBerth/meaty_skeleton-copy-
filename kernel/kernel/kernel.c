#include <stdio.h>

#include <kernel/fb_tty.h>
#include <drivers/keyboard.h>
#include <games/morpion.h>
#include <drivers/network.h>

void kernel_main(char* multiboot_struct) {
	framebuffer_initialize(multiboot_struct);
	fb_terminal_setup ();
	card_setup ();	
}
