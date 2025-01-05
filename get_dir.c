#include "get_dir.h"

//=============================================================================
uint8_t get_dir32(int8_t dx, int8_t dy) __naked
{
	dx; dy;
__asm
	pop		bc
	pop		hl
	push	hl
	push	bc		// L = dx, H = dy

_get_dir32_stub:
	ld		e, #0b00100000	// E = dir

	xor		a, a
	sub		a, h
	jr		z, _get_dir_ret_dy0
	jp		p, _get_dir_skip1
	ld		e, #0b01011000	// E = dir
	neg
_get_dir_skip1:
	ld		d, a			// D = dy

	xor		a, a
	sub		a, l
	jr		z, _get_dir_ret_dx0
	jp		p, _get_dir_skip2
	srl		e
	srl		e
	neg
_get_dir_skip2:
	ld		l, a			// L = dx
	ld		h, d			// H = dy

	ld		a, #0x18
	and		a, e

	bit		3, a
	jr		z, _get_dir_calc
	ld		d, h
	ld		h, l
	ld		l, d			// swap dx and dy
	jr		_get_dir_calc

_get_dir_ret_dy0:
	bit		7, l
	jr		z, _get_dir_ret_dy0_1
	ld		l, #0x00		// dir = 0
	ret
_get_dir_ret_dy0_1:
	ld		l, #0x10		// dir = 16
	ret

_get_dir_ret_dx0:
	bit		7, h
	jr		z, _get_dir_ret_dx0_1
	ld		l, #0x08		// dir = 8
	ret

_get_dir_ret_dx0_1:
	ld		l, #0x18		// dir = 24
	ret

_get_dir_calc:
	ex		af, af			// push A (dir)

	ld		e, l
	ld		d, #0
	ld		l, h
	ld		h, #0			// HL = dy, DE = dx

	ld		a, l
	sub		a, e
	jr		nc, _get_dir_skip4

	// case (dy < dx)
	call	_get_dir_sub
	ld		l, a
	ex		af, af
	add		a, l
	ld		l, a
	ret

_get_dir_skip4:
	// case (dy >= dx)
	ex		de, hl
	call	_get_dir_sub
	ld		l, a
	ex		af, af
	add		a, #8
	sub		a, l
	and		a, #0x1f
	ld		l, a
	ret

_get_dir_sub:
	xor		a, a		// A=0, CY=0
	ld		c, l
	ld		b, h
	add		hl, hl		// dy * 2
	push	hl
	add		hl, bc		// dy * 3
	pop		bc
	push	bc			// *1 dy * 2
	push	hl			// *2 dy * 3
	add		hl, bc		// dy * 5
	add		hl, hl		// dy * 10
	sbc		hl, de
	jr		c, _get_dir_ret0
	inc		a
	pop		hl			// *2, HL = dy * 3
	ld		c, l
	ld		b, h
	sbc		hl, de
	jr		c, _get_dir_ret1
	inc		a
	pop		hl			// *1, HL = dy * 2
	sbc		hl, de
	jr		c, _get_dir_ret2
	inc		a
	ld		l, c
	ld		h, b
	add		hl, hl		// dy * 6
	ex		de, hl
	ld		c, l
	ld		b, h
	add		hl, hl
	add		hl, hl
	add		hl, bc		// dx * 5
	sbc		hl, de
	adc		a, #0
	ret

_get_dir_ret0:
	pop		hl
_get_dir_ret1:
	pop		hl
_get_dir_ret2:
	ret
__endasm;
}
