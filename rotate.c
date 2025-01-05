#include "n80dev.h"

#ifdef USE_ROTATE_SMALL
//=============================================================================
uint32_t rotate_small(int16_t x, int16_t y) __naked
{
	x; y;
__asm
	ld		(_restore_ix+2), ix

	pop		bc
	pop		ix
	pop		iy

	push	iy
	push	ix
	push	bc

	push	iy
	ld		hl, #1
	add		hl, sp		// HL = SP+1
	ld		a, (hl)		// A = IYH
	rlca
	sbc		a, a		// A = (A < 0) ? 0xFF : 0x00
	rrd
	dec		hl
	rrd					// (SP) = IY >> 4 (in stack)
	ex		de, hl
	pop		bc			// BC = IY >> 4
	push	ix
	pop		hl			// HL = IX
	or		a
	sbc		hl, bc		// HL = IX - (IY >> 4) {c}
	ld		b, h
	ld		a, l
	sra		b
	rra
	sra		b
	rra
	sra		b
	rra
	ld		c, a		// BC = HL >> 3
	add		iy, bc		// IY = IY + (HL >> 3) {d}
	push	iy
	ex		de, hl
	inc		hl
	ld		a, (hl)
	rlca
	sbc		a, a
	rrd
	dec		hl
	rrd					// (SP) = new_IY >> 4 (= {d} >> 4)
	ex		de, hl
	pop		bc
	or		a
	sbc		hl, bc		// HL = {c} - ({d} >> 4)
	push	hl

	//pop		ix
	pop		hl		// HL = new x
	push	iy
	pop		de		// DE = new y

_restore_ix:
	ld		ix, #0
	ret
__endasm;
}
#endif

#if 0
//=============================================================================
uint32_t rotate90_org(uint8_t dir, int16_t x, int16_t y)
{
	struct {
		int16_t x;
		int16_t y;
	} work;

	int16_t tmp;
	switch ((dir >> 3) & 3) {
	case 1:
		tmp = x;
		x = -y;
		y = tmp;
		break;
	case 2:
		x = -x;
		y = -y;
		break;
	case 3:
		tmp = x;
		x = y;
		y = -tmp;
		break;
	default:
		break;
	}

	int16_t c;

	dir &= 7;
	switch (dir) {
	default:
		break;
	case 1:
		c = x - (y * 2) / 16;
		y = (c * 3) / 16 + y;
		x = c - (y * 2) / 16;
		break;
	case 2:
		c = x - (y * 3) / 16;
		y = (c * 3) / 8 + y;
		x = c - (y * 3) / 16;
		break;
	case 3:
		c = x - ((uint32_t)y * 5) / 16;
		y = ((uint32_t)c * 9) / 16 + y;
		x = c - ((uint32_t)y * 5) / 16;
		break;
	case 4:
		c = x - ((uint32_t)y * 7) / 16;
		y = ((uint32_t)c * 11) / 16 + y;
		x = c - ((uint32_t)y * 7) / 16;
		break;
	case 5:
		c = x - ((uint32_t)y * 9) / 16;
		y = ((uint32_t)c * 13) / 16 + y;
		x = c - ((uint32_t)y * 9) / 16;
		break;
	case 6:
		c = x - ((uint32_t)y * 11) / 16;
		y = ((uint32_t)c * 15) / 16 + y;
		x = c - ((uint32_t)y * 11) / 16;
		break;
	case 7:
		c = x - ((uint32_t)y * 13) / 16;
		y = c + y;
		x = c - ((uint32_t)y * 13) / 16;
		break;
	}

	work.x = x;
	work.y = y;
	return (*(uint32_t *)&work);

	//return (((uint32_t)y << 16) | (uint32_t)x);
}
#endif

//=============================================================================
uint32_t rotate32(int16_t x, int16_t y, uint8_t dir) __naked
{
	dir; x; y;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)	// HL = x
	ld		e, 4 (iy)
	ld		d, 5 (iy)	// DE = y

	ld		c, 6 (iy)	// C = dir

	bit		4, c
	jr		z, _rotate90_skip1

	// HL = -HL
	xor		a, a
	sub		a, l
	ld		l, a
	sbc		a, h
	sub		a, l
	ld		h, a

	// DE = -DE
	xor		a, a
	sub		a, e
	ld		e, a
	sbc		a, d
	sub		a, e
	ld		d, a

_rotate90_skip1:
	bit		3, c
	jr		z, _rotate90_skip2

	ld		c, l
	ld		b, h

	xor		a, a
	sub		a, e
	ld		l, a
	sbc		a, d
	sub		a, l
	ld		h, a

	ld		e, c
	ld		d, b

_rotate90_skip2:

	ld		a, 6 (iy)	// A = dir
	and		a, #7
	ret		z

	ld		c, a
	add		a, a
	add		a, c

	ld		c, a
	ld		b, #0
	ld		iy, #_rotate90_jmp
	add		iy, bc
	jp		(iy)

_rotate90_jmp:
	jp		_rotate_1
	jp		_rotate_1
	jp		_rotate_2
	jp		_rotate_3
	jp		_rotate_4
	jp		_rotate_5
	jp		_rotate_6
	jp		_rotate_7
__endasm;
}

//=============================================================================
static uint32_t rotate_1() __naked
{
__asm
	// HL = x
	// DE = y

	call	_rotate_m_2_16	// c = x - (y * 2) / 16

	ex		de, hl
	ld		a, e
	ld		b, d
	sra		b
	rra
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	add		hl, bc
	sra		b
	rra
	ld		c, a
	add		hl, bc		// y = y + (c * 3) / 16
	ex		de, hl

	// x = c - (y * 2) / 16

_rotate_m_2_16:
	// res = HL - (DE * 2) / 16
	ld		a, e
	ld		b, d
	sra		b
	rra
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc

	ret
__endasm;
}

//=============================================================================
static uint32_t rotate_2() __naked
{
__asm
	// HL = x
	// DE = y

	call	_rotate_m_3_16	// c = x - (y * 3) / 16;

	// DE = DE + (HL * 6) / 16
	ex		de, hl
	ld		a, e
	ld		b, d
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	add		hl, bc
	sra		b
	rra
	ld		c, a
	add		hl, bc
	ex		de, hl

_rotate_m_3_16:
	// HL = HL - (DE * 3) / 16
	ld		a, e
	ld		b, d
	sra		b
	rra
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc
	ret
__endasm;
}

//=============================================================================
static uint32_t rotate_3() __naked
{
__asm
	// HL = x
	// DE = y

	call	_rotate_m_5_16

	// DE = DE + (HL * 9) / 16
	ex		de, hl
	ld		a, e
	ld		b, d
	sra		b
	rra
	ld		c, a
	add		hl, bc
	sra		b
	rra
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	add		hl, bc
	ex		de, hl

_rotate_m_5_16:
	// HL = HL - (DE * 5) / 16
	ld		a, e
	ld		b, d
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc

	ret
__endasm;
}

//=============================================================================
static uint32_t rotate_4() __naked
{
__asm
	// HL = x
	// DE = y

	call	_rotate_m_7_16

	// DE = DE + (HL * 11) / 16
	ex		de, hl
	ld		a, e
	ld		b, d
	sra		b
	rra
	ld		c, a
	add		hl, bc
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	add		hl, bc 
	sra		b
	rra
	ld		c, a
	add		hl, bc 
	ex		de, hl

_rotate_m_7_16:
	// HL = HL - (DE * 7) / 16
	ld		a, e
	ld		b, d
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc
	sra		b
	rra
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	add		hl, bc

	ret
__endasm;
}

//=============================================================================
static uint32_t rotate_5() __naked
{
__asm
	// HL = x
	// DE = y

	call	_rotate_m_9_16

	// DE = DE + (HL * 13) / 16
	ex		de, hl
	ld		a, e
	ld		b, d
	sra		b
	rra
	ld		c, a
	add		hl, bc
	sra		b
	rra
	ld		c, a
	add		hl, bc
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	add		hl, bc
	ex		de, hl

_rotate_m_9_16:
	// HL = HL - (DE * 9) / 16
	ld		a, e
	ld		b, d
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc
	sra		b
	rra
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc

	ret
__endasm;
}

//=============================================================================
static uint32_t rotate_6() __naked
{
__asm
	// HL = x
	// DE = y

	call	_rotate_m_11_16

	// DE = DE + (HL * 15) / 16
	ex		de, hl
	ld		a, e
	ld		b, d
	sra		b
	rra
	sra		b
	rra
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	sbc		hl, bc
	add		hl, de
	ex		de, hl

_rotate_m_11_16:
	// HL = HL - (DE * 11) / 16
	ld		a, e
	ld		b, d
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc

	ret
__endasm;
}

//=============================================================================
static uint32_t rotate_7() __naked
{
__asm
	// HL = x
	// DE = y

	call	_rotate_m_13_16

	// DE = DE + HL
	ex		de, hl
	add		hl, de
	ex		de, hl

_rotate_m_13_16:
	// HL = HL - (DE * 13) / 16
	ld		a, e
	ld		b, d
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc
	sra		b
	rra
	sra		b
	rra
	ld		c, a
	or		a, a
	sbc		hl, bc

	ret
__endasm;
}
