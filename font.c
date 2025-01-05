#include "n80dev.h"

#ifndef FNT_FONT_W
#define FNT_FONT_W 3
#endif
#ifndef FNT_FONT_H
#define FNT_FONT_H 2
#endif

#ifdef USE_FONT_UNDERLINE
static const uint8_t bmp_underscore[] = {
  0x00, 0x00, 0x00,
  0x88, 0x88, 0x88
};
#endif
static Bitmap_t bmp_font;

//=============================================================================
void font_init(const uint8_t *_font_array) __z88dk_fastcall __naked
{
	_font_array;
	//bmp_font.w = FNT_FONT_W;
	//bmp_font.h = FNT_FONT_H;
__asm
	ld		(_font_put_array_addr+1), hl

	ld		hl, #(FNT_FONT_H * 0x100) + FNT_FONT_W
	ld		(_bmp_font), hl

#if (FNT_FONT_W != 3)
	xor		a, a
	ld		(_font_put_array_mul3), a
#endif

	ret
__endasm;
}

//=============================================================================
// 0x00 <= ch < 0x40
const uint8_t *font_addr(uint8_t ch) __preserves_regs(b, c, iyh, iyl) __z88dk_fastcall __naked
{
	ch;
__asm
	ld		a, l
	add		a, a
_font_put_array_mul3:
	add		a, l
	add		a, a
	ld		l, a
	ld		a, #0
	adc		a, a
	ld		h, a
_font_put_array_addr:
	ld		de, #0 //_font_array
	add		hl, de
	ret
__endasm;
}

//=============================================================================
void font_put(uint8_t x, uint8_t y, uint8_t ch, uint8_t color) __naked
{
#if 0
	uint8_t flag;
	if (ch & 0x80) {
		ch -= 0xa0;
		flag = 1;
	} else {
		ch -= 0x20;
		flag = 0;
	}
	if (ch < 0x40) {
		bmp_font.data = font_array + ((uint16_t)ch * 6);
		put_bitmap(x, y, &bmp_font, color);
#ifdef USE_FONT_UNDERLINE
		if (flag) {
			bmp_font.data = (uint8_t *)bmp_underscore;
			or_bitmap(x, y, &bmp_font, color);
		}
#endif
		if (flag) {
			invert_rect(x, y, 3, 2);
		}
	}
#else
	x; y; ch; color;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		a, 6 (ix) // ch
#ifdef BMP_USE_INVERT_RECT
	cp		#0x80
	push	af
#endif

	and		a, #0x7f
	sub		a, #0x20
	cp		#0x40
	jr		nc, _font_put_1

	ld		l, a
	call	_font_addr

	ld		((_bmp_font + 0x0002)), hl

	ld		a, 7 (ix) // color
	push	af
	inc		sp
	ld		hl, #_bmp_font
	push	hl

	ld		l, 4 (ix) // x
	ld		h, 5 (ix) // y
	push	hl
	call	_put_bitmap
	pop		af
	pop		af
	inc		sp

#ifdef BMP_USE_INVERT_RECT
	pop		af
	jr		c, _font_put_1
	ld		hl, (_bmp_font)
	push	hl
	ld		l, 4 (ix)
	ld		h, 5 (ix)
	push	hl
	call	_invert_rect
	pop		af
	pop		af
#endif

_font_put_1:
	pop		ix
	ret
__endasm;
#endif
}

//=============================================================================
void font_puts(uint8_t x, uint8_t y, const uint8_t *str, uint8_t color) __naked
{
	x; y; str; color;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		h, 4 (ix)
	ld		l, 5 (ix)
	call	_get_offscr_addr
	ex		de, hl

	ld		l, 6 (ix)
	ld		h, 7 (ix)

_font_puts_loop:
	ld		a, (hl)
	inc		hl
	or		a, a
	jr		z, _font_puts_attr

	and		a, #0x7f
	sub		a, #0x20
	cp		#0x40
	jr		nc, _font_puts_skip

	push	hl
	push	de

	ld		l, a
	call	_font_addr

	pop		de
	push	de
	ld		c, #FNT_FONT_W
	ld		b, #FNT_FONT_H
	call	_put_bitmap_vram

	pop		de
	pop		hl

_font_puts_skip:
#if (FNT_FONT_W == 3)
	inc		de
#endif
	inc		de
	inc		de
	jr		_font_puts_loop

_font_puts_attr:
	ld		a, 8 (ix)
	or		a, a
	jr		z, _font_puts_ret

	ld		d, a

	ld		l, 6 (ix)
	ld		h, 7 (ix)
	ld		c, #0
	xor		a, a
	cpir
	ld		a, #0xff
	sub		a, c
#if (FNT_FONT_W == 3)
	ld		c, a
#endif
	add		a, a
#if (FNT_FONT_W == 3)
	add		a, c
#endif
	ld		e, a

	push	de
	ld		h, 4 (ix)
	ld		l, 5 (ix)
	push	hl
	call	_insert_attr
	pop		hl
	inc		l
	push	hl
	call	_insert_attr

_font_puts_ret:
	ld		sp, ix
	pop		ix
	ret	
__endasm;
}

#if 0
//=============================================================================
void font_puts(uint8_t x, uint8_t y, const uint8_t *str, uint8_t color) __naked
{
#if 0
	uint8_t ch, w;

	w = 0;
	while ((ch = *str ++)) {
		font_put(x, y, ch, 0);
		x += 3;
		w += 3;
	}
	if (color != 0) {
		fill_rect_attr(((x-w) << 8) | y, w, 2, color);
	}
#else
	x; y; str; color;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		l, 4 (ix)	// L = x
	ld		h, 5 (ix)	// H = y
	ld		e, 6 (ix)
	ld		d, 7 (ix)	// DE = str

	ld		a, (_bmp_font)
	ld		b, a		// B = font.w
	ld		c, #0
_font_puts_1:
	ld		a, (de)
	or		a, a
	jr		z, _font_puts_2

	push	hl
	push	de
	push	bc

	ld		b, #0
	ld		c, a
	push	bc
	push	hl
	call	_font_put
	pop		af
	pop		af

	pop		bc
	pop		de
	pop		hl

	inc		de
	ld		a, c
	add		a, b
	ld		c, a		// C += font.w
	ld		a, l
	add		a, b
	ld		l, a		// L += font.w
	jr		_font_puts_1

_font_puts_2:
	ld		a, 8 (ix)	// A = attr
	and		a, a
	jr		z, _font_puts_3

	ld		d, a
	ld		e, c

	ld		a, l
	ld		l, h
	sub		a, c
	ld		h, a

	ld		a, (_bmp_font+1)
	ld		b, a	// b = font.h

_font_puts_loop:
	push	bc
	push	de		// (attr, w)
	push	hl		// (x, y)
	call	_insert_attr
	pop		hl		// WARNING (may not same)
	pop		de		// WARNING (may not same)
	pop		bc

	inc		l
	djnz	_font_puts_loop

_font_puts_3:
	pop		ix
	ret
__endasm;
#endif
}
#endif
