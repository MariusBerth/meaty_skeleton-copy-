#include <stdint.h>
#include <string.h>
#include <kernel/fb_tty.h>
#include <drivers/keyboard.h>
#include <games/morpion.h>
#include <shell/shell.h>

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
            if (pos > 0) {
              text[pos--] = 0;
              //supprimer la derniere entree
              fb_removechar();
            };
          }
        else if (pos < 200)
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

    //effectue l'entree

    for(pos=0; !(text[pos] == 0) && !(text[pos] == ' '); pos++);
    text[pos++] = 0;

    if (!(memcmp("echo",text,5))) {
      echo(text + pos);
    }
    else if (!(memcmp("morpion",text,8))) {
      morpion();
      fb_terminal_setup();
    }
    else if (!(memcmp("clear",text,6))) {
      fb_terminal_setup();
    }
    else if (!(memcmp("exit",text,5))) {
      break;
    }
    else {
      fb_writestring(text);
      fb_writestring(" : commande introuvable\n");
    }

    ;
    /*
    fb_writestring(text);
    fb_putchar('\n');
    */
  };
}
