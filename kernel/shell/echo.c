#include <kernel/fb_tty.h>
void echo(char * text){
  fb_writestring(text);
  fb_putchar('\n');
}
