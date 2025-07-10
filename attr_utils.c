#include "n80dev.h"

//=============================================================================
#ifdef BMP_USE_FILL_HLINES_ATTR
void fill_hlines_attr(uint8_t start_line, uint8_t num_lines, uint8_t color)
{
    start_line; num_lines; color;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		l, 4 (ix)		// L = start_line
	ld		h, #80
    call    _get_offscr_addr

	ld		a, 6 (ix)		// A = color
_fill_hlines_attr_1:
	ld		(hl), #0
	inc		hl
	ld		(hl), a
	inc		hl
	ld		(hl), #80
	inc		hl
	ld		(hl), #NO_ATTR // 0xe8
	ld		e, l
	ld		d, h
	inc		de
	dec		hl			// HL = (attr top) + 2, DE = (attr top) + 4
	ld		bc, #(ATTRS_PER_LINE * 2)-4
#if (ATTRS_PER_LINE != 20)
	ldir
#else
	ldi
	ldi
	ldi
	ldi
_fill_hlines_attr_ldi:
	ldi
	ldi
	ldi
	ldi
	ldi
	ldi
	ldi
	ldi
	jp		pe, _fill_hlines_attr_ldi
	// 616
#endif
	ld		hl, #80
	add		hl, de

	dec		5 (ix)			// num_lines --
	jr		nz, _fill_hlines_attr_1

	pop		ix
__endasm;
}
#endif

//=============================================================================
#ifdef BMP_USE_FILL_RECT_ATTR
void fill_rect_attr(uint16_t xy, uint8_t w, uint8_t h, uint8_t color) __naked
{
	xy; w; h; color;
__asm
	ld		iy, #0
	add		iy, sp

	ld		h, 6 (iy) // color
	ld		l, 4 (iy) // w

	ld		d, 3 (iy) // x
	ld		e, 2 (iy) // y

	ld		b, 5 (iy) // h

_fill_rect_attr_1:
	push	bc

	push	hl
	push	de
	call	_insert_attr
	pop		de		// WARNING (may not same)
	pop		hl		// WARNING (may not same)

	pop		bc

	inc		e
	djnz	_fill_rect_attr_1

	ret
__endasm;

#if 0
	uint8_t line, x;

	line = xy & 0xff;
	x = xy >> 8;
	for (; h > 0; h --, line ++) {
		insert_attr(line, x, w, color);
	}
#endif
}
#endif
