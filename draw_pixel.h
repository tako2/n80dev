#ifndef _DRAW_PIXEL_H
#define _DRAW_PIXEL_H

#include <stddef.h>
#include <stdint.h>

extern void draw_pixel(uint8_t x, uint8_t y);

#ifdef USE_CLEAR_PIXEL
extern void clear_pixel(uint8_t x, uint8_t y);
#endif

#endif
