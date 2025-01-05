#include "n80dev.h"

//=============================================================================
int8_t bcd24_compare(uint8_t *bcd1, uint8_t *bcd2) __naked
{
	bcd1; bcd2;
__asm
	ld		hl, #2
	add		hl, sp

	ld		e, (hl)
	inc		hl
	ld		d, (hl)
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a
	// DE = bcd1 (2 added), HL = bcd2 (2 added)
	ld		b, #3

_bcd_comp_loop:
	ld		a, (de)
	sub		a, (hl)
	jr		c, _bcd_comp_ret_minus
	jr		z, _bcd_comp_skip

	ld		l, #1
	ret

_bcd_comp_skip:
	dec		de
	dec		hl
	djnz	_bcd_comp_loop

	ld		l, #0
	ret

_bcd_comp_ret_minus:
	ld		l, #-1
	ret
__endasm;
}
