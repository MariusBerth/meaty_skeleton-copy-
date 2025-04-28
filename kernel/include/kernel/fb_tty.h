#ifndef _KERNEL_FB_TTY_H
#define _KERNEL_FB_TTY_H

#include <stddef.h>
#include <stdint.h>
struct color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

extern struct color FB_RED, FB_GREEN, FB_BLUE, FB_WHITE, FB_BLACK;

uint32_t get_color (struct color color);

void framebuffer_initialize(char* multiboot_struct);
void put_pixel (uint32_t x, uint32_t y, uint32_t truecolor);
void fillrect (uint32_t x, uint32_t y, const uint32_t w, const uint32_t h,
		struct color color_bytes);

void fb_putchar (uint32_t x, uint32_t y, struct color color, 
		unsigned char c);

#endif
