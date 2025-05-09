#include <kernel/fb_tty.h>
#include <string.h>
void set_color(char * arg){
  int c;
  if (!(memcmp("f ",arg,2))){
      c = 1;
  }
  else if (!(memcmp("b ",arg,2))){
      c = 0;
  }
  else {
    fb_writestring("mauvais premier argument, f ou b attendu\n");
    return;
  };
  struct color color;
  char * argsuiv = arg + 2;
  if (!(memcmp("black",argsuiv,6))) {
    color=FB_BLACK;
  }
  else if (!(memcmp("white",argsuiv,6))) {
    color=FB_WHITE;
  }
  else if (!(memcmp("blue",argsuiv,5))) {
    color=FB_BLUE;
  }
  else if (!(memcmp("red",argsuiv,4))) {
    color=FB_RED;
  }
  else if (!(memcmp("green",argsuiv,6))) {
    color=FB_GREEN;
  }
  else if (!(memcmp("grey",argsuiv,5))) {
    color=FB_GREY;
  }
  else {
    fb_writestring("couleur non reconnue\n");
    return;
  };

  if (c) {
    fb_set_font_color(color);
  }
  else {
    fb_set_bg_color(color);
    uint32_t rang = (get_fb_row()) << 4;
    fillrect(0, rang, FB_WIDTH, FB_HEIGHT - rang, color);
  };
}
