#ifndef _80MKII_H
#define _80MKII_H

#include <stddef.h>
#include <stdint.h>

#include "bitmap.h"

extern void n80_init_vram() __preserves_regs(h, l, d, e, b, c, iyh, iyl);
extern void n80_set_bgcol(uint8_t col) __z88dk_fastcall __preserves_regs(d, e, b, c, iyh, iyl);
extern void n80_enter_vram() __preserves_regs(h, l, d, e, b, c, iyh, iyl);
extern void n80_exit_vram() __preserves_regs(h, l, d, e, b, c, iyh, iyl);
extern uint8_t *n80_vram_addr(uint16_t xy) __z88dk_fastcall __preserves_regs(iyh, iyl);
extern void n80_draw_bitmap(uint8_t x, uint8_t y, const Bitmap_t *bmp);
extern void n80_draw_hline(uint8_t x, uint8_t y, uint8_t w, uint8_t pat);
extern void n80_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t pat);
extern void n80_fill_rect32(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t pat);

extern void n80_draw_kanji(uint8_t x, uint8_t y, uint16_t addr);
extern void n80_draw_kanji_str(uint8_t x, uint8_t y, const uint16_t *str);

#define n80_data_start() __asm \
	.area		_EXTCODE (ABS) \
__endasm
#define n80_data_end() __asm \
	.area		_CODE \
__endasm

#endif // !_80MKII_H
