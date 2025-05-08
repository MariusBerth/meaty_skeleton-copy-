#include <stdint.h>
#include <kernel/fb_tty.h>
#include <drivers/keyboard.h>


void shell(void)
{
char text [200];
while(1)
  {
    fb_writestring("> : ");
    int pos = 0;
    char entree;
    text[0]=0;
    while ((entree = keyboard_read()) != 28 /*'\n'*/)
      {
        if (entree == 14)
          {
            text[pos--] = 0;
            //supprimer la derniere entree
            fb_removechar();
          }
        else
          {
            entree = scancode_to_char(entree);
            //enregistrer l'entrée
            text[pos++] = entree;
            text[pos] = 0;
            //afficher l'entree
            fb_putchar(entree);
          };
      };
    fb_putchar('\n');
    fb_writestring(text);
    fb_putchar('\n');
  };
}
