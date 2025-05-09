#include <string.h>

int str_to_int(char* i) {
  int r=0;
  while(*i != 0){
    if (*i=='1'){r=10*r+1;}
    else if (*i=='2'){r=10*r+2;}
    else if (*i=='3'){r=10*r+3;}
    else if (*i=='4'){r=10*r+4;}
    else if (*i=='5'){r=10*r+5;}
    else if (*i=='6'){r=10*r+6;}
    else if (*i=='7'){r=10*r+7;}
    else if (*i=='8'){r=10*r+8;}
    else if (*i=='9'){r=10*r+9;}
    else if (*i=='0'){r=10*r;}
    i++;
  };
  return r;
}
