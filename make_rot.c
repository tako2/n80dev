#include "n80dev.h"

#define si16(v) ((int16_t)((v) << 8))
#define sh8(v) ((int8_t)((v) >> 8))

//=============================================================================
static uint8_t get_px(uint8_t x, uint8_t y, const uint8_t *ptr) __naked
{
	x; y; ptr;
__asm
	pop		bc
	pop		hl
	pop		de
	push	de
	push	hl
	push	bc

	// L = x, H = y, DE = ptr
_get_px_stub:
	ld		a, #8-1
	ld		c, l
	ld		l, #0
	cp		c
	ret		c
	cp		h
	ret		c

	call	_px_ptr

	ld		a, (de)
	and		a, b
	ld		l, a
	ret

_px_ptr:
	// in: C = x, H = y, DE = ptr
	// out: B = bit, DE = ptr
	ld		a, h
	and		a, #0x03
	ld		(_get_px_jmp+1), a
	ld		a, #0x88
_get_px_jmp:
	jr		_get_px_jmp
	rrca
	rrca
	rrca
	// A = 1 << (y & 3)
	ld		b, a

	srl		c
	sbc		a, a
	cpl
	xor		a, #0xf0	// CY=1,A=0xf0 / CY=0,A=0x0f

	and		a, b
	ld		b, a		// B = ((x & 1) ? 0x10 : 0x01) << (y & 3)

	ld		a, h
	and		a, #0xfc
	add		a, c

	add		a, e
	ld		e, a
	adc		a, d
	sub		a, e
	ld		d, a		// DE += (y & 0xfc) + (x >> 1)
	ret
__endasm;

#if 0
	uint8_t bit;

	if (x >= 8 || y >= 8) return (0x00);

	bit = (x & 1) ? 0x10 : 0x01;
	bit <<= (y & 3);

	ptr += (y & 0xfc) + (x >> 1);

	return (*ptr & bit);
#endif
}

//=============================================================================
static void set_px(uint8_t x, uint8_t y, uint8_t *ptr) __naked
{
	x; y; ptr;
__asm
	pop		bc
	pop		hl
	pop		de
	push	de
	push	hl
	push	bc

	// L = x, H = y, DE = ptr
_set_px_stub:
	ld		a, #8-1
	ld		c, l
	cp		c
	ret		c
	cp		h
	ret		c

	call	_px_ptr

	ld		a, (de)
	or		a, b
	ld		(de), a
	ret
__endasm;

#if 0
	uint8_t bit;

	if (x >= 8 || y >= 8) return;

	bit = (x & 1) ? 0x10 : 0x01;
	bit <<= (y & 3);

	ptr += (y & 0xfc) + (x >> 1);

	*ptr |= bit;
#endif
}

//=============================================================================
void make_rot_ch(const uint8_t *src, uint8_t *dest, uint8_t dir) __naked
{
#if 1
	src; dest; dir;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		a, #32
	sub		a, 8 (ix)
	and		a, #31
	ld		h, a	// H = (32 - dir) & 31

	ld		b, #-3	// B = y = -3

_make_rot_ch_loop2:
	ld		c, #-3	// C = x = -1

_make_rot_ch_loop1:
	push	hl
	push	bc

	push	hl
	inc		sp

	ld		l, #0
	ld		h, b	// HL = si16(y)
	push	hl
	ld		h, c	// HL = si16(x)
	push	hl

	call	_rotate32

	pop		af
	pop		af
	inc		sp

	ld		a, h
	add		a, #3
	ld		l, a	// L = sh8(vec.x)+3
	ld		a, d
	add		a, #3
	ld		h, a	// H = sh8(vec.y)+3

	ld		e, 4 (ix)
	ld		d, 5 (ix)	// DE = src

	call	_get_px_stub

	ld		a, l
	or		a, a
	jr		z, _make_rot_ch_skip1

	pop		bc
	push	bc

	ld		a, c
	add		a, #3
	ld		l, a	// L = x + 3
	ld		a, b
	add		a, #3
	ld		h, a	// H = y + 3

	ld		e, 6 (ix)
	ld		d, 7 (ix)

	call	_set_px_stub

_make_rot_ch_skip1:
	pop		bc
	pop		hl

	inc		c
	ld		a, c
	add		a, #3
	cp		#7
	jr		c, _make_rot_ch_loop1

	inc		b
	ld		a, b
	add		a, #3
	cp		#7
	jr		c, _make_rot_ch_loop2

	pop		ix
	ret
__endasm;

#else
	int8_t x, y;
	Vector_t vec;

	dir = (32 - dir) & 31;

	for (y = -3; y < 4; y ++) {
		for (x = -3; x < 4; x ++) {
			vec.val = rotate32(si16(x), si16(y), dir);

			if (get_px(sh8(vec.x)+3, sh8(vec.y)+3, src)) {
				set_px(x+3, y+3, dest);
			}
		}
	}
#endif
}
