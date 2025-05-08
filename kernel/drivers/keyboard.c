#include "keyboard.h"
#include <kernel/asm_functions.h>
//#define KEYBD		(uint8_t*) 0x60	/* I/O port for keyboard data */

//https://wiki.osdev.org/Inline_Assembly/Examples#INx
/*static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}
*/

uint16_t KEYBD = (uint16_t) 0x60;
uint8_t keyboard_read(void)
{
  uint8_t lu = inb(KEYBD);
  //*KEYBD = 0;
  while (lu == inb(KEYBD))
    {io_wait();};
  //lu = *KEYBD;
  lu = inb(KEYBD);
  if (0x80 & lu)
    {lu=keyboard_read();};
  return lu;
}
char table[128] = "##1234567890-=#\tqwertyuiop[]\n#asdfghjkl;\'`#\\zxcvbnm,./#*# #####";

char scancode_to_char(uint8_t o)
{
return table[o];
}
