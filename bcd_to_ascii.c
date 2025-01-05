#include "n80dev.h"

//=============================================================================
void bcd_to_ascii(uint8_t *bcd, uint8_t keta, uint8_t *dest) __naked
{
	bcd; keta; dest;
__asm
	ld		iy, #0
	add		iy, sp

	ld		e, 2 (iy)
	ld		d, 3 (iy)	// DE = bcd
	ld		b, 4 (iy)	// B = keta
	ld		l, 5 (iy)
	ld		h, 6 (iy)	// HL = dest

_bcd_to_ascii_reg::
	// DE = bcd, B = keta, HL = dest

	ld		a, b
	dec		a
	add		a, e
	ld		e, a
	adc		a, d
	sub		a, e
	ld		d, a		// DE = DE + (keta-1)

	ld		c, #0x00

_bcd_to_ascii_5:
	ld		a, (de)
	rlca
	rlca
	rlca
	rlca
	and		a, #0x0f
	cp		c
	jr		z, _bcd_to_ascii_1
	ld		c, #0x30
	or		a, c
	ld		(hl), a
	jr		_bcd_to_ascii_2
_bcd_to_ascii_1:
	ld		(hl), #0x20
_bcd_to_ascii_2:
	inc		hl

	ld		a, (de)
	dec		de
	and		a, #0x0f
	cp		c
	jr		z, _bcd_to_ascii_3
	ld		c, #0x30
	or		a, c
	ld		(hl), a
	jr		_bcd_to_ascii_4
_bcd_to_ascii_3:
	ld		(hl), #0x20
_bcd_to_ascii_4:
	inc		hl

	djnz	_bcd_to_ascii_5

	ld		(hl), #0

	ld		a, c
	and		a, a
	ret		nz

	dec		hl
	ld		(hl), #0x30
	ret
__endasm;
}
