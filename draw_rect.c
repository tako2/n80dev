#include "n80dev.h"

//=============================================================================
#ifdef BMP_USE_FILL_RECT
void fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t ch, uint8_t color) __naked
{
	x; y; w; h; ch; color;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		h, 5 (ix)	// H = y
	ld		l, 7 (ix)	// L = h
	call	_clip_vline
	push	hl			// *1

	ld		b, 4 (ix)	// B = x
	ld		c, 6 (ix)	// C = w
	call	_clip_hline

	pop		de			// *1 D = y, E = h

	ld		h, b
	ld		l, d		// H = x, L = y
	ld		b, e		// B = height, C = width

	ld		a, 9 (ix)	// color
	and		a, a
	jr		z, _fill_rect_0

	// _fill_rect_attr
	push	hl
	push	bc

	ld		d, a		// D = attr
	ld		e, c		// E = width

_fill_rect_loop:
	push	bc

	push	de
	push	hl
	call	_insert_attr
	pop		hl			// WARNING (may not same)
	pop		de			// WARNING (may not same)

	inc		l
	pop		bc
	djnz	_fill_rect_loop

	pop		bc
	pop		hl
	// _fill_rect_attr

_fill_rect_0:
	push	bc
	call	_get_offscr_addr
	pop		bc

	ld		a, 8 (ix)	// A = ch

	// if c(width) == 1 then _fill_rect_1
	dec		c
	jr		z, _fill_rect_1

_fill_rect_2:
	ld		(hl), a
	ld		d, h
	ld		e, l
	inc		de

	push	bc
	push	hl
	ld		b, #0
	ldir
	pop		hl
	ld		c, #120
	add		hl, bc
	pop		bc

	djnz	_fill_rect_2

	pop		ix
	ret

_fill_rect_1:
	ld		(hl), a

	// if B(height) == 1 then _fill_rect_ret
	ld		c, b
	dec		c
	jr		z, _fill_rect_ret
	ld		b, #0

	ld		de, #120
	ex		de, hl
	add		hl, de
	ex		de, hl

	ldir

_fill_rect_ret:
	pop		ix
	ret
__endasm;
}
#endif

//=============================================================================
#ifdef BMP_USE_INVERT_RECT
void invert_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	x; y; w; h;

__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		b, 4 (ix) // x
	ld		c, 6 (ix) // w
	call	_clip_hline

	ld		a, #120
	sub		a, c // _skip_dest
	ld		e, a
	ld		d, #0
	push	de
	ld		6 (ix), c // w

	ld		l, 5 (ix) // y
	ld		h, b // x
	call	_get_offscr_addr // hl = dest

	pop		de
	ld		c, 7 (ix) // h
_invert_rect_loop2:
	ld		b, 6 (ix) // w
_invert_rect_loop1:
	ld		a, (hl)
	cpl
	ld		(hl), a
	inc		hl
	djnz	_invert_rect_loop1

	add		hl, de
	dec		c
	jr	nz, _invert_rect_loop2

_invert_rect_ret:
	pop		ix
__endasm;
}
#endif

