#ifndef _KERNEL_FB_TTY_H
#define _KERNEL_FB_TTY_H

#include <stddef.h>
#include <stdint.h>
struct color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

extern struct color FB_RED, FB_GREEN, FB_BLUE, FB_WHITE, FB_BLACK, FB_GREY;
extern struct color FB_DARK_RED, FB_DARK_GREEN, FB_DARK_BLUE;
extern struct color FB_MAGENTA, FB_YELLOW, FB_CYAN;

extern uint32_t FB_WIDTH, FB_HEIGHT;

uint32_t get_color (struct color color);

void framebuffer_initialize(char* multiboot_struct);
void put_pixel (uint32_t x, uint32_t y, uint32_t truecolor);
void fillrect (uint32_t x, uint32_t y, const uint32_t w, const uint32_t h,
		struct color color_bytes);
void bresenham_jesken_circle (uint32_t cx, uint32_t cy, 
		uint32_t r, struct color color);

void fb_terminal_setup (void);
void fb_terminal_clear (void);
void fb_set_font_color(struct color color);
void fb_set_bg_color(struct color color);
struct color fb_get_font_color(void);
struct color fb_get_bg_color(void);
uint32_t get_fb_row(void);
void fb_putentryat (uint32_t x, uint32_t y, struct color color, 
		char c);
void fb_putchar (char c);
void fb_removechar (void);
void fb_write (char* str, size_t size);
void fb_writestring (char* str);
void fb_writeat (char* str, uint32_t x, uint32_t y, 
		struct color color);

#endif
