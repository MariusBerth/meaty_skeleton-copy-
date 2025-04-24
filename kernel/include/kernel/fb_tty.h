#ifndef _KERNEL_FB_TTY_H
#define _KERNEL_FB_TTY_H

#include <stddef.h>
#include <stdint.h>

void framebuffer_initialize(char* multiboot_struct);
void put_pixel (uint32_t x, uint32_t y, uint32_t r, uint32_t g, uint32_t b);
void fillrect (uint32_t x, uint32_t y, const uint32_t w, const uint32_t h,
                 uint32_t r, uint32_t g, uint32_t b);

#endif
