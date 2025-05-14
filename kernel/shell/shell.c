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
    fb_drawcursor();
    while ((entree = keyboard_read()) != 28 /*'\n'*/)
      {
        if (entree == 14)
          {
            if (pos > 0) {
              text[--pos] = 0;
              //supprimer la derniere entree
              fb_removecursor();
              fb_removechar();
              fb_drawcursor();
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
            fb_drawcursor();
          };
      };
    fb_removecursor();
    fb_putchar('\n');

    //effectue l'entree

    for(pos=0; !(text[pos] == 0) && !(text[pos] == ' '); pos++);
    text[pos++] = 0;

    if (!(memcmp("echo",text,5))) {
      echo(text + pos);
    }
    else if (!(memcmp("morpion",text,8))) {
      morpion();
      fb_terminal_clear();
    }
    else if (!(memcmp("morpionhost",text,12))) {
      morpion_host();
      fb_terminal_clear();
    }
    else if (!(memcmp("morpioninvite",text,14))) {
      morpion_invite();
      fb_terminal_clear();
    }
    else if (!(memcmp("set",text,4))) {
      set_color(text + pos);
    }
    else if (!(memcmp("clear",text,6))) {
      fb_terminal_clear();
    }
    else if (!(memcmp("clean",text,6))) {
      fb_terminal_setup();
    }
    else if (!(memcmp("exit",text,5))) {
      break;
    }
    else if (!(memcmp("send",text,5))) {
      send (text + pos);
    }
    else if (!(memcmp("receive",text,8))) {
      receive ();
    }
    else if (!(memcmp("help",text,5))) {
      fb_writestring("liste des commandes :\n");
      fb_writestring("- echo : recopie ce qui est fourni en argument\n");
      fb_writestring("- morpion : lance un morpion\n");
      fb_writestring("- morpionhost : lance un morpion en tant qu'hote\n");
      fb_writestring("- morpioninvite : rejoint le morpion cree par l'autre emulateeur\n");
      fb_writestring("- set : permet de changer les couleurs de la police (f) et du fond (b) selon des couleurs predefinies ou rgb\n");
      fb_writestring("- clear : efface tout ce qui est ecrit\n");
      fb_writestring("- clean : comme clean, mais reinitialise aussi les couleurs\n");
      fb_writestring("- send : envoie un message a l'autre emulateur\n");
      fb_writestring("- recieve : recoit un message de l'autre emulateur\n");
      fb_writestring("- exit : sort du shell\n");
      fb_writestring("- help : affiche cette page\n");
    }
    else {
      fb_writestring(text);
      fb_writestring(" : commande introuvable\n");
    };

    /*
    fb_writestring(text);
    fb_putchar('\n');
    */
  };
}
