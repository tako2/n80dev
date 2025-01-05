#ifndef __FONT_H
#define __FONT_H

#include <stddef.h>
#include <stdint.h>

//=============================================================================
typedef struct {
	const uint8_t *msg;
	uint8_t x;
	uint8_t y;
} MsgList_t;

//=============================================================================
extern void font_init(const uint8_t *_font_array) __z88dk_fastcall;
extern const uint8_t *font_addr(uint8_t ch) __preserves_regs(b, c, iyh, iyl) __z88dk_fastcall;
extern void font_put(uint8_t x, uint8_t y, uint8_t ch, uint8_t color);
extern void font_puts(uint8_t x, uint8_t y, const uint8_t *str, uint8_t color);

extern void font_put_num(uint8_t x, uint8_t y, uint16_t num, uint8_t keta, uint8_t color);

extern void sfnt_puts(uint8_t x, uint8_t y, const uint8_t *str);
extern void sfnt_put_bcd(uint8_t x, uint8_t y, uint8_t *bcd, uint8_t keta);
extern void sfnt_make_bmp(const uint8_t *str, uint8_t *dest);

extern void draw_msg_list(const MsgList_t *list) __z88dk_fastcall;

#endif
