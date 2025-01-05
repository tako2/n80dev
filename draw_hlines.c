#include "n80dev.h"

extern void _fill_line();

//=============================================================================
void draw_hlines(uint8_t y, uint8_t h, uint16_t ch) __naked
{
    y; h; ch;
__asm
	push	ix
    ld      ix, #0
    add     ix, sp

    ld      l, 4 (ix) // l = y
    ld      h, #0
    call    _get_offscr_addr

	ld		c, 6 (ix) // c = ch(l)
	ld		b, 7 (ix) // b = ch(h)

	ld		a, 5 (ix) // a = h

_draw_hlines_1:
	call	__fill_line
	ld		de, #40
	add		hl, de
	dec		a
	jr		nz, _draw_hlines_1

#ifdef COUNT_VRTC_IN_LIB
	call	_count_vrtc
#endif

	pop 	ix
	ret
__endasm;
}
