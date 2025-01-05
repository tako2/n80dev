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

//=============================================================================
#if 0
int8_t bcd_compare(uint8_t *val1, uint8_t *val2, uint8_t keta) __naked
{
	val1; val2; keta;
__asm
	ld		iy, #0
	add		iy, sp

	ld		e, 2 (iy)
	ld		d, 3 (iy)
	ld		l, 4 (iy)
	ld		h, 5 (iy)

	ld		c, 6 (iy)
	ld		b, #0
	dec		c

	add		hl, bc
	ex		de, hl
	add		hl, bc

	inc		c
	ld		b, c

_bcd_compare_1:
	ld		a, (de)
	sub		a, (hl)
	jr		c, _bcd_compare_2
	jr		z, _bcd_compare_3

	ld		l, #0xff
	ret

_bcd_compare_3:
	dec		hl
	dec		de
	djnz	_bcd_compare_1

	ld		l, #0
	ret

_bcd_compare_2:
	ld		l, #0x01
	ret
__endasm;
}
#endif
