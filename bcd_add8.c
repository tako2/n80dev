#include "n80dev.h"

//=============================================================================
uint8_t bcd_add8(uint8_t *dest, uint8_t keta, uint8_t val) __preserves_regs(d, e, iyh, iyl) __naked
{
	dest; keta; val;
__asm
	ld		hl, #5
	add		hl, sp

	ld		a, (hl)		// A = val
	dec		hl
	ld		b, (hl)		// B = keta
	dec		hl
	ld		c, (hl)
	dec		hl
	ld		l, (hl)
	ld		h, c		// HL = dest

	or		a, a
	jr		_bcd_add8_skip1

_bcd_add8_loop:
	inc		hl
	ld		a, #0

_bcd_add8_skip1:
	adc		a, (hl)
	daa
	ld		(hl), a

	djnz	_bcd_add8_loop

_bcd_add8_ret:
	rla
	and		a, #1
	ld		l, a
	ret
__endasm;
}
