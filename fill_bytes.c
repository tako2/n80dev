#include "n80dev.h"

void fill_bytes(uint8_t x, uint8_t y, uint8_t w, uint8_t ch, uint8_t attr) __naked
{
	x; y; w; ch; attr;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 3 (iy)
	ld		h, 2 (iy)
	call	_get_offscr_addr	// HL = vram

	ld		c, 4 (iy)		// C = w
	ld		a, 5 (iy)		// A = ch

	ld		(hl), a
	ld		e, l
	ld		d, h
	inc		de
	dec		c
	jr		z, _fill_bytes_skip1
	ld		b, #0
	ldir

_fill_bytes_skip1:
	ld		a, 6 (iy)		// A = attr
	or		a, a
	ret		z

	ld		h, a			// H = attr
	ld		l, 4 (iy)		// L = w
	push	hl
	ld		h, 2 (iy)		// H = x
	ld		l, 3 (iy)		// L = y
	push	hl
	call	_insert_attr
	pop		af
	pop		af

	ret
__endasm;
}
