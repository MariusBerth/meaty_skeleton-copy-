#ifndef _KEYBOARD_DRIVER
#define  _KEYBOARD_DRIVER

uint8_t keyboard_read(void);
char scancode_to_char(uint8_t);

#endif
