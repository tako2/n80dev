#include "n80dev.h"

//=============================================================================
uint8_t bcd_add16(uint8_t *dest, uint8_t keta, uint16_t val) __preserves_regs(d, e) __naked
{
	dest; keta; val;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)
	ld		b, 4 (iy)
	dec		b			// must be more than 2
	ret		z
	ld		a, 5 (iy)

	add		a, (hl)
	daa
	ld		(hl), a
	inc		hl

	ld		a, 6 (iy)
	adc		a, (hl)
	daa
	ld		(hl), a
	inc		hl

	djnz	_bcd_add16_1
	jr		_bcd_add16_ret

_bcd_add16_1:
	ld		a, #0
	adc		a, (hl)
	daa
	ld		(hl), a
	inc		hl

	djnz	_bcd_add16_1
_bcd_add16_ret:
	rla
	and		a, #1
	ld		l, a
	ret
__endasm;
}
