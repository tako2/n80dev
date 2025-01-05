#include "n80dev.h"

//=============================================================================
void font_put_bcd(uint8_t x, uint8_t y, uint8_t *bcd, uint8_t keta) __naked
{
	x; y; bcd; keta;
__asm
	ld		iy, #0
	add		iy, sp

	ld		h, 2 (iy)
	ld		l, 3 (iy)
	call	_get_offscr_addr	// HL = VRAM addr

	ld		e, 4 (iy)
	ld		d, 5 (iy)		// DE = bcd

	ld		b, 6 (iy)		// B = keta

	ld		a, b
	dec		a
	add		a, e
	ld		e, a
	adc		a, d
	sub		a, e
	ld		d, a			// DE = DE + (keta - 1)

	ld		c, #0x00
_sfnt_put_bcd_1:
	ld		a, (de)
	rlca
	rlca
	rlca
	rlca
	and		a, #0x0f
	cp		c
	jr		z, _sfnt_put_bcd_2
	ld		c, #0x10
	or		a, c
	call	_sfnt_putch
	jr		_sfnt_put_bcd_3
_sfnt_put_bcd_2:
	inc		hl
	inc		hl
_sfnt_put_bcd_3:
	ld		a, (de)
	dec		de
	and		a, #0x0f
	cp		c
	jr		z, _sfnt_put_bcd_4
	ld		c, #0x10
	or		a, c
	call	_sfnt_putch
	jr		_sfnt_put_bcd_5
_sfnt_put_bcd_4:
	inc		hl
	inc		hl
_sfnt_put_bcd_5:
	djnz	_sfnt_put_bcd_1

	ld		a, c
	and		a, a
	ret		nz

	dec		hl
	dec		hl
	ld		a, #0x10
	call	_sfnt_putch
	ret
__endasm;
}
