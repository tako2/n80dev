#include "n80dev.h"

//=============================================================================
#ifdef BMP_USE_DRAW_PATLINES
void draw_patlines(uint8_t y, uint8_t h, uint8_t skip, uint8_t color,
				   uint8_t *pat)
{
	y; h; skip; color; pat;
__asm
    push    ix
    ld      ix, #0
    add     ix, sp

    ld      l, 4 (ix)
    ld      h, #0
    call    _get_offscr_addr

    ld      e, 8 (ix)
    ld      d, 9 (ix)
    ex      de, hl
	// de = VRAM addr, hl = pat

    ld      a, 5 (ix)

_draw_patlines_0:
    ld      bc, #80
_draw_patlines_1:
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
	jp		pe, _draw_patlines_1

#ifdef CLEAR_ATTR
	push	af
	push	hl

	ex		de, hl
	ld		(hl), #0
	inc		hl
	ld      a, 7 (ix) // color
	ld		(hl), a
	inc		hl
	ld		(hl), #80
	ld		e, l
	ld		d, h
	inc		de
	dec		hl
	ld		bc, #40-3
	ldir

	pop		hl
	pop		af
#else
	ex		de, hl
	ld		bc, #40
	add		hl, bc
	ex		de, hl
#endif

    ld      c, 6 (ix)
	ld      b, #0
    add     hl, bc

    dec     a
    jr      nz, _draw_patlines_0

#ifdef COUNT_VRTC_IN_LIB
	call	_count_vrtc
#endif

    pop     ix
    ret
__endasm;
}
#endif	// BMP_USE_DRAW_PATLINES
