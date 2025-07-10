#include "n80dev.h"

#ifdef REC_NUM_LINES
uint8_t num_lines[2];
#endif

//=============================================================================
const uint8_t pixel_bit[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
//const uint8_t pixel_bit[] = { 0x01, 0x01, 0x01, 0x01, 0x10, 0x10, 0x10, 0x10 };

//=============================================================================
void draw_pixel(uint8_t x, uint8_t y) __naked
{
	x; y;
__asm
	ld		hl, #2
	add		hl, sp

	ld		d, (hl)	// D = x
	inc		hl
	ld		e, (hl)	// E = y

	ld		a, e
	and		a, #3
	bit		0, d
	jr		z, _draw_pixel_skip1
	add		a, #4
_draw_pixel_skip1:
	add		a, #<_pixel_bit
	ld		l, a
	ld		a, #0
	adc		a, #>_pixel_bit
	ld		h, a
	ld		a, (hl)
	ex		af, af

	srl		d
	srl		e
	srl		e
	ex		de, hl
	call	_get_offscr_addr

	ex		af, af
	or		a, (hl)
	ld		(hl), a

	ret
__endasm;
#if 0
	uint8_t *vram;
	uint8_t ch;

	ch = pixel_bit[(y & 3) * ((x & 1) ? 2 : 1)];

	x >>= 1;
	y >>= 2;
	vram = get_offscr_addr((x << 8) | y);

	*vram |= ch;
#endif
}

//=============================================================================
void clear_pixel(uint8_t x, uint8_t y) __naked
{
	x; y;
__asm
	ld		hl, #2
	add		hl, sp

	ld		d, (hl)	// D = x
	inc		hl
	ld		e, (hl)	// E = y

	ld		a, e
	and		a, #3
	bit		0, d
	jr		z, _clear_pixel_skip1
	add		a, #4
_clear_pixel_skip1:
	add		a, #<_pixel_bit
	ld		l, a
	ld		a, #0
	adc		a, #>_pixel_bit
	ld		h, a
	ld		a, (hl)
	ex		af, af

	srl		d
	srl		e
	srl		e
	ex		de, hl
	call	_get_offscr_addr

	ex		af, af
	cpl
	and		a, (hl)
	ld		(hl), a

	ret
__endasm;
}

//=============================================================================
typedef void *draw_pixel_t (uint8_t, uint8_t);
draw_pixel_t *draw_func;

//=============================================================================
void get_vram_bit() __naked
{
	// Input:
	//   H = x, L = y
	// Output:
	//   HL : VRAM address
	//    C : bit pattern
__asm
	xor		a, a
	srl		l
	adc		a, #1
	srl		l
	jr		nc, _get_vram_bit_skip2
	add		a, a
	add		a, a
_get_vram_bit_skip2:

	srl		h
	jr		nc, _get_vram_bit_skip3
	add		a, a
	add		a, a
	add		a, a
	add		a, a
_get_vram_bit_skip3:

	ld		c, a
	call	_get_offscr_addr
	ret
__endasm;
}

//=============================================================================
void get_vram_bity() __naked
{
	// Input:
	//   H = x, L = y
	// Output:
	//   HL : VRAM address
	//    C : bit pattern 1 (lower 4bit)
	//    B : bit pattern 2 (higher 4bit)
__asm
	xor		a, a
	srl		l
	adc		a, #1
	srl		l
	jr		nc, _get_vram_bity_skip2
	add		a, a
	add		a, a
_get_vram_bity_skip2:
	ld		c, a
	rlca
	rlca
	rlca
	rlca
	and		a, #0xf0
	ld		b, a

	srl		h
	jp		_get_offscr_addr
__endasm;
}

#if 0
uint8_t bit_x(uint8_t bit) __z88dk_fastcall __naked
{
__asm
	ld		a, l
	rlca
	rlca
	rlca
	rlca
	ld		l, a
	ret
__endasm;
}
#endif

//=============================================================================
void next_bitx() __naked
{
__asm
	ld		a, c
	rlca
	rlca
	rlca
	rlca
	ld		c, a
	and		a, #0x0f
	ret		z
	inc		hl
	ret
__endasm;
}

//=============================================================================
void draw_hline_ym(uint8_t x0, uint8_t y0, uint8_t dx, uint8_t dy) __naked
{
	x0; y0; dx; dy;
__asm
	push	bc
	call	_get_vram_bity
	ld		e, c
	ld		d, b		// E = b1, D = b2

	ld		bc, #-BYTES_PER_LINE	// BC = -120

	exx
	pop		de
	//ld		e, 4 (iy)	// E' = dx
	ld		b, e
	inc		b			// B' = num_x = dx + 1
	ld		c, e
	srl		c			// C' = d = (dx / 2)
	//ld		d, 5 (iy)	// D' = dy
	exx

	//bit		0, 2 (iy)
	bit		0, l
	jr		z, _draw_hline_ym_skip1

	// (x0 & 1) != 0
	ld		a, (hl)
	or		a, d
	ld		(hl), a
	inc		hl			// *vram ++ |= b2

	exx
	ld		a, c
	sub		a, d
	ld		c, a		// d -= dy
	dec		b			// num_x --
	ret		z
	exx

_draw_hline_ym_skip1:
	exx
_draw_hline_ym_loop1:
	bit		7, c		// if (d < 0)
	jr		z, _draw_hline_ym_skip2_1

	// d < 0
	ld		a, c
	add		a, e
	ld		c, a		// d += dx

	exx

	bit		0, e		// b1 == 0x01
	jr		z, _draw_hline_ym_skip3

	// b1 == 0x01
	add		hl, bc
	ld		de, #0x8008	// b1 = 0x08, b2 = 0x80
	jr		_draw_hline_ym_skip2

_draw_hline_ym_skip3:
	// b1 != 0x01
	srl		e
	srl		d

_draw_hline_ym_skip2:
	exx

_draw_hline_ym_skip2_1:
	dec		b
	jr		nz, _draw_hline_ym_skip4
	exx
	ld		d, #0	// b2 = 0
	exx

_draw_hline_ym_skip4:
	ld		a, c
	sub		a, d
	ld		c, a	// d -= dy

	jr		nc, _draw_hline_ym_skip5

	// d < 0
	add		a, e
	ld		c, a	// d += dx

	exx

	// add_y < 0
	bit		4, d	// b2 == 0x10
	jr		nz, _draw_hline_ym_skip6

	// b2 != 0x10
	srl		d
	ld		a, d
	or		a, e
	or		a, (hl)
	ld		(hl), a
	inc		hl
	srl		e
	jr		_draw_hline_ym_skip7

_draw_hline_ym_skip6:
	// b2 == 0x10
	ld		d, #0x80
	ld		a, e
	or		a, (hl)
	ld		(hl), a
	add		hl, bc
	ld		a, d
	or		a, (hl)
	ld		(hl), a
	inc		hl
	ld		e, #0x08
	jr		_draw_hline_ym_skip7

_draw_hline_ym_skip5:
	exx
	ld		a, d
	or		a, e
	or		a, (hl)
	ld		(hl), a
	inc		hl

_draw_hline_ym_skip7:
	ld		a, d
	or		a, a
	ret		z

	exx
	dec		b		// num_x --
	ret		z

	ld		a, c
	sub		a, d
	ld		c, a	// d -= dy

	jp		_draw_hline_ym_loop1
__endasm;
}

//=============================================================================
void draw_hline_yp(uint8_t x0, uint8_t y0, uint8_t dx, uint8_t dy) __naked
{
	x0; y0; dx; dy;
__asm
	push	bc
	call	_get_vram_bity
	ld		e, c
	ld		d, b		// E = b1, D = b2

	ld		bc, #BYTES_PER_LINE		// BC = 120

	exx
	pop		de
	//ld		e, 4 (iy)	// E' = dx
	ld		b, e
	inc		b			// B' = num_x = dx + 1
	ld		c, e
	srl		c			// C' = d = (dx / 2)
	//ld		d, 5 (iy)	// D' = dy
	exx

	//bit		0, 2 (iy)
	bit		0, l
	jr		z, _draw_hline_yp_skip1

	// (x0 & 1) != 0
	ld		a, (hl)
	or		a, d
	ld		(hl), a
	inc		hl			// *vram ++ |= b2

	exx
	ld		a, c
	sub		a, d
	ld		c, a		// d -= dy
	dec		b			// num_x --
	ret		z
	exx

_draw_hline_yp_skip1:
	exx
_draw_hline_yp_loop1:
	bit		7, c		// if (d < 0)
	jr		z, _draw_hline_yp_skip2_1

	// d < 0
	ld		a, c
	add		a, e
	ld		c, a		// d += dx

	exx

	// add_y > 0
	bit		3, e		// b1 == 0x08
	jr		z, _draw_hline_yp_skip3_2

	// b1 == 0x08
	add		hl, bc
	ld		de, #0x1001	// b1 = 0x01, b2 = 0x10
	jr		_draw_hline_yp_skip2

_draw_hline_yp_skip3_2:
	// b1 != 0x08
	sla		e
	sla		d

_draw_hline_yp_skip2:
	exx

_draw_hline_yp_skip2_1:
	dec		b
	jr		nz, _draw_hline_yp_skip4
	exx
	ld		d, #0	// b2 = 0
	exx

_draw_hline_yp_skip4:
	ld		a, c
	sub		a, d
	ld		c, a	// d -= dy

	jr		nc, _draw_hline_yp_skip5

	// d < 0
	add		a, e
	ld		c, a	// d += dx

	exx

	// add_y >= 0
	bit		7, d	// b2 == 0x80
	jr		nz, _draw_hline_yp_skip6_2

	// b2 != 0x80
	sla		d
	ld		a, d
	or		a, e
	or		a, (hl)
	ld		(hl), a
	inc		hl
	sla		e
	jr		_draw_hline_yp_skip7

_draw_hline_yp_skip6_2:
	// b2 == 0x80
	ld		d, #0x10
	ld		a, e
	or		a, (hl)
	ld		(hl), a
	add		hl, bc
	ld		a, d
	or		a, (hl)
	ld		(hl), a
	inc		hl
	ld		e, #0x01
	jr		_draw_hline_yp_skip7

_draw_hline_yp_skip5:
	exx
	ld		a, d
	or		a, e
	or		a, (hl)
	ld		(hl), a
	inc		hl

_draw_hline_yp_skip7:
	ld		a, d
	or		a, a
	ret		z

	exx
	dec		b		// num_x --
	ret		z

	ld		a, c
	sub		a, d
	ld		c, a	// d -= dy

	jp		_draw_hline_yp_loop1
__endasm;
}

//=============================================================================
#if 0
void draw_hline(uint8_t x0, uint8_t y0,
				uint8_t dx, uint8_t dy, int8_t add_y) __naked
{
	x0; y0; dx; dy; add_y;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 3 (iy)	// L = y0
	ld		h, 2 (iy)	// H = x0

	call	_get_vram_bity
	ld		e, c
	ld		d, b		// E = b1, D = b2

	bit		7, 6 (iy)	// if add_y is plus or minus
	jr		nz, _draw_hline_skip0_1
	ld		bc, #BYTES_PER_LINE		// BC = 120
	jr		_draw_hline_skip0_2
_draw_hline_skip0_1:
	ld		bc, #-BYTES_PER_LINE	// BC = -120
_draw_hline_skip0_2:

	exx
	ld		e, 4 (iy)	// E' = dx
	ld		b, e
	inc		b			// B' = num_x = dx + 1
	ld		c, e
	srl		c			// C' = d = (dx / 2)
	ld		d, 5 (iy)	// D' = dy
	exx

	bit		0, 2 (iy)
	jr		z, _draw_hline_skip1

	// (x0 & 1) != 0
	ld		a, (hl)
	or		a, d
	ld		(hl), a
	inc		hl			// *vram ++ |= b2

	exx
	ld		a, c
	sub		a, d
	ld		c, a		// d -= dy
	dec		b			// num_x --
	ret		z
	exx

_draw_hline_skip1:
	exx
_draw_hline_loop1:
	bit		7, c		// if (d < 0)
	jr		z, _draw_hline_skip2_1

	// d < 0
	ld		a, c
	add		a, e
	ld		c, a		// d += dx

	exx

	bit		7, b		// if (add_y < 0)
	jr		z, _draw_hline_skip3_1

	bit		0, e		// b1 == 0x01
	jr		z, _draw_hline_skip3

	// b1 == 0x01
	add		hl, bc
	ld		de, #0x8008	// b1 = 0x08, b2 = 0x80
	jr		_draw_hline_skip2

_draw_hline_skip3:
	srl		e
	srl		d
	jr		_draw_hline_skip2

_draw_hline_skip3_1:
	bit		3, e		// b1 == 0x08
	jr		z, _draw_hline_skip3_2

	// b1 == 0x08
	add		hl, bc
	ld		de, #0x1001	// b1 = 0x01, b2 = 0x10
	jr		_draw_hline_skip2

_draw_hline_skip3_2:
	sla		e
	sla		d

_draw_hline_skip2:
	exx

_draw_hline_skip2_1:
	dec		b
	jr		nz, _draw_hline_skip4
	exx
	ld		d, #0	// b2 = 0
	exx

_draw_hline_skip4:
	ld		a, c
	sub		a, d
	ld		c, a	// d -= dy

	jr		nc, _draw_hline_skip5

	// d < 0
	add		a, e
	ld		c, a	// d += dx

	exx

	bit		7, b	// if (add_y < 0)
	jr		z, _draw_hline_skip6_1

	// add_y < 0
	bit		4, d	// b2 == 0x10
	jr		nz, _draw_hline_skip6

	// b2 != 0x10
	srl		d
	ld		a, d
	or		a, e
	or		a, (hl)
	ld		(hl), a
	inc		hl
	srl		e
	jr		_draw_hline_skip7

_draw_hline_skip6:
	ld		d, #0x80
	ld		a, e
	or		a, (hl)
	ld		(hl), a
	add		hl, bc
	ld		a, d
	or		a, (hl)
	ld		(hl), a
	inc		hl
	ld		e, #0x08
	jr		_draw_hline_skip7

_draw_hline_skip6_1:
	// add_y >= 0
	bit		7, d	// b2 == 0x80
	jr		nz, _draw_hline_skip6_2

	// b2 != 0x80
	sla		d
	ld		a, d
	or		a, e
	or		a, (hl)
	ld		(hl), a
	inc		hl
	sla		e
	jr		_draw_hline_skip7

_draw_hline_skip6_2:
	ld		d, #0x10
	ld		a, e
	or		a, (hl)
	ld		(hl), a
	add		hl, bc
	ld		a, d
	or		a, (hl)
	ld		(hl), a
	inc		hl
	ld		e, #0x01
	jr		_draw_hline_skip7

_draw_hline_skip5:
	exx
	ld		a, d
	or		a, e
	or		a, (hl)
	ld		(hl), a
	inc		hl

_draw_hline_skip7:
	ld		a, d
	or		a, a
	ret		z

	exx
	dec		b		// num_x --
	ret		z

	ld		a, c
	sub		a, d
	ld		c, a	// d -= dy

	jp		_draw_hline_loop1
__endasm;

#if 0
	uint8_t *vram;
	int8_t d;
	uint8_t num_x;
	uint8_t b1, b2;

	vram = get_offscr_addr(((x0 & 0xfe) << 7) | (y0 >> 2));
	b1 = 1 << (y0 & 3);
	b2 = b1 << 4;

	d = (dx / 2);
	num_x = dx + 1;

	if (x0 & 1) {
		*vram ++ |= b2;
		d -= dy;
		num_x --;
	}

	while (num_x != 0) {
		if (d < 0) {
			d += dx;
			if (add_y < 0) {
				if (b1 == 0x01) {
					vram -= 120;
					b1 = 0x08;
					b2 = 0x80;
				} else {
					b1 >>= 1;
					b2 >>= 1;
				}
			} else {
				if (b1 == 0x08) {
					vram += 120;
					b1 = 0x01;
					b2 = 0x10;
				} else {
					b1 <<= 1;
					b2 <<= 1;
				}
			}
		}

		num_x --;
		if (num_x == 0) {
			b2 = 0;
		}
		d -= dy;

		if (d < 0) {
			d += dx;
			if (add_y < 0) {
				if (b2 != 0x10) {
					b2 >>= 1;
					*vram ++ |= (b1 | b2);
					b1 >>= 1;
				} else {
					b2 = 0x80;
					*vram |= b1;
					vram -= 120;
					*vram ++ |= b2;
					b1 = 0x08;
				}
			} else {
				if (b2 != 0x80) {
					b2 <<= 1;
					*vram ++ |= (b1 | b2);
					b1 <<= 1;
				} else {
					b2 = 0x10;
					*vram |= b1;
					vram += 120;
					*vram ++ |= b2;
					b1 = 0x01;
				}
			}
		} else {
			*vram ++ |= (b1 | b2);
		}
		if (b2 == 0) return;

		d -= dy;
		num_x --;
	}
#endif
}
#endif

//=============================================================================
#if 0
void draw_hline(uint8_t x0, uint8_t y0,
				uint8_t dx, uint8_t dy, int8_t add_y)
{
#if 0
	uint8_t *vram;
	uint8_t bit;
	int8_t d;

	vram = get_offscr_addr(((x0 & 0xfe) << 7) | (y0 >> 2));
	bit = 1 << (y0 & 3);
	if (x0 & 1) {
		bit <<= 4;
	}

	d = (dx / 2) - dy;
	*vram |= bit;
	num_x --;
	bit = bit_x(bit);
	if (bit & 0x0f) vram ++;
	while (num_x != 0) {
		if (d < 0) {
			if (add_y < 0) {
				if (bit & 0x11) {
					vram -= 120;
					bit <<= 3;
				} else {
					bit >>= 1;
				}
			} else {
				if (bit & 0x88) {
					vram += 120;
					bit >>= 3;
				} else {
					bit <<= 1;
				}
			}
			*vram |= bit;
			d -= dy - dx;
		} else {
			*vram |= bit;
			d -= dy;
		}
		bit = bit_x(bit);
		if (bit & 0x0f) vram ++;
		num_x --;
	}
#endif

#if 1
	x0; y0; dx; dy; add_y;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 3 (iy)	// L = y0
	ld		h, 2 (iy)	// H = x0

	call	_get_vram_bit

	// HL = vram addr, C = bit pat

	ld		b, 4 (iy)	// B = dx
	inc		b
	ld		de, #BYTES_PER_LINE

	exx
	ld		l, 4 (iy)	// L = dx
	ld		h, 5 (iy)	// H = dy
	ld		a, l
	srl		a
	sub		a, h
	ld		e, a		// E = (dx / 2) - dy
	ld		a, h
	sub		a, l
	ld		l, a		// L = dy - dx
	ld		d, 6 (iy)	// D = add_y
	exx

_draw_hline_loop:
	ld		a, (hl)
	or		a, c
	ld		(hl), a

	call	_next_bitx

	dec		b
	ret		z

	exx
	bit		7, e
	jr		z, _draw_hline_skip4

	// case (d < 0)
	ld		a, e
	sub		a, l
	ld		e, a

	bit		7, d
	exx
	jr		z, _draw_hline_skip5

	// case (add_y < 0)
	ld		a, c
	and		a, #0x11
	jr		z, _draw_hline_skip6
	sbc		hl, de
	sla		c
	sla		c
	sla		c
	jr		_draw_hline_loop
_draw_hline_skip6:
	srl		c
	jr		_draw_hline_loop

_draw_hline_skip5:
	// case (add_y > 0)
	ld		a, c
	and		a, #0x88
	jr		z, _draw_hline_skip8
	add		hl, de
	srl		c
	srl		c
	srl		c
	jr		_draw_hline_loop
_draw_hline_skip8:
	sla		c
	jr		_draw_hline_loop

_draw_hline_skip4:
	// case (d >= 0)
	ld		a, e
	sub		a, h
	ld		e, a
	exx

	jr		_draw_hline_loop
__endasm;
#endif
}
#endif

//=============================================================================
void next_bity() __naked
{
__asm
	ld		a, c
	and		a, #0x88
	jr		z, _next_bity_skip
	add		hl, de
	srl		c
	srl		c
	srl		c
	ret
_next_bity_skip:
	sla		c
	ret
__endasm;
}

//=============================================================================
void draw_vline_xm(uint8_t x0, uint8_t y0, uint8_t dx, uint8_t dy) __naked
{
	x0; y0; dx; dy;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 3 (iy)	// L = Y0
	ld		h, 2 (iy)	// H = X0

	call	_get_vram_bit
						// HL = vram, C = bit

	ld		b, 5 (iy)	// B = dy (num_y)
	ld		a, b
	srl		a			// A' = dy / 2 (d)
	ex		af, af
	inc		b

	ld		d, 4 (iy)	// D = dx

	ld		e, c		// E = C (dest = bit)

	dec		b
	jr		z, _draw_vline_xm_ret

_draw_vline_xm_loop1:
	ld		a, c
	rlca
	ld		c, a
	and		a, #0x11
	jr		z, _draw_vline_xm_skip1

	// bit(C) is looped
	xor		a, #0x11
	ld		c, a

	ld		a, e
	or		a, (hl)
	ld		(hl), a
	ld		e, #0

	// HL += BYTES_PER_LINE
	ld		a, #BYTES_PER_LINE
	add		a, l
	ld		l, a
	adc		a, h
	sub		a, l
	ld		h, a

_draw_vline_xm_skip1:
	ex		af, af		// * A'
	sub		a, d
	jr		nc, _draw_vline_xm_skip3

	// (d < 0)
	add		a, 5 (iy)	// A' += dy
	ex		af, af		// * A

	// (add_x < 0)
	ld		a, #0x0f
	and		a, c
	jr		z, _draw_vline_xm_skip5

	// (bit & 0x0f) != 0
	ld		a, e
	or		a, (hl)
	ld		(hl), a
	ld		e, #0
	dec		hl

	// bit(C) <<= 4
	ld		a, c
	rlca
	rlca
	rlca
	rlca
	ld		c, a

	jr		_draw_vline_xm_skip7

_draw_vline_xm_skip5:
	// bit(C) >>= 4
	ld		a, c
	rrca
	rrca
	rrca
	rrca
	ld		c, a

	ex		af, af		// * A'

_draw_vline_xm_skip3:
	ex		af, af		// * A

_draw_vline_xm_skip7:
	// (d >= 0)
	ld		a, e
	or		a, c
	ld		e, a

	djnz	_draw_vline_xm_loop1

_draw_vline_xm_ret:
	ld		a, e
	or		a, a
	ret		z
	or		a, (hl)
	ld		(hl), a
	ret
__endasm;
}

//=============================================================================
void draw_vline_xp(uint8_t x0, uint8_t y0, uint8_t dx, uint8_t dy) __naked
{
	x0; y0; dx; dy;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 3 (iy)	// L = Y0
	ld		h, 2 (iy)	// H = X0

	call	_get_vram_bit
						// HL = vram, C = bit

	ld		b, 5 (iy)	// B = dy (num_y)
	ld		a, b
	srl		a			// A' = dy / 2 (d)
	ex		af, af
	inc		b

	ld		d, 4 (iy)	// D = dx

	ld		e, c		// E = C (dest = bit)

	dec		b
	jr		z, _draw_vline_xp_ret

_draw_vline_xp_loop1:
	ld		a, c
	rlca
	ld		c, a
	and		a, #0x11
	jr		z, _draw_vline_xp_skip1

	// bit(C) is looped
	xor		a, #0x11
	ld		c, a

	ld		a, e
	or		a, (hl)
	ld		(hl), a
	ld		e, #0

	// HL += BYTES_PER_LINE
	ld		a, #BYTES_PER_LINE
	add		a, l
	ld		l, a
	adc		a, h
	sub		a, l
	ld		h, a

_draw_vline_xp_skip1:
	ex		af, af		// * A'
	sub		a, d
	jr		nc, _draw_vline_xp_skip3

	// (d < 0)
	add		a, 5 (iy)	// A' += dy
	ex		af, af		// * A

	// (add_x >= 0)
	ld		a, #0xf0
	and		a, c
	jr		z, _draw_vline_xp_skip6

	// (bit & 0xf0) != 0
	ld		a, e
	or		a, (hl)
	ld		(hl), a
	ld		e, #0
	inc		hl

	// bit(C) >>= 4
	ld		a, c
	rrca
	rrca
	rrca
	rrca
	ld		c, a

	jr		_draw_vline_xp_skip7

_draw_vline_xp_skip6:
	// bit(C) <<= 4
	ld		a, c
	rlca
	rlca
	rlca
	rlca
	ld		c, a

	ex		af, af		// * A'

_draw_vline_xp_skip3:
	ex		af, af		// * A

_draw_vline_xp_skip7:
	// (d >= 0)
	ld		a, e
	or		a, c
	ld		e, a

	djnz	_draw_vline_xp_loop1

_draw_vline_xp_ret:
	ld		a, e
	or		a, a
	ret		z
	or		a, (hl)
	ld		(hl), a
	ret
__endasm;
}

#if 0
//=============================================================================
void draw_vline(uint8_t x0, uint8_t y0, uint8_t dx, uint8_t dy, int8_t add_x) __naked
{
	x0; y0; dx; dy; add_x;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 3 (iy)	// L = Y0
	ld		h, 2 (iy)	// H = X0

	call	_get_vram_bit
						// HL = vram, C = bit

	ld		b, 5 (iy)	// B = dy (num_y)
	ld		a, b
	srl		a			// A' = dy / 2 (d)
	ex		af, af
	inc		b

	ld		d, 4 (iy)	// D = dx

	ld		e, c		// E = C (dest = bit)

	dec		b
	jr		z, _draw_vline_ret

_draw_vline_loop1:
	ld		a, c
	rlca
	ld		c, a
	and		a, #0x11
	jr		z, _draw_vline_skip1

	// bit(C) is looped
	xor		a, #0x11
	ld		c, a

	ld		a, e
	or		a, (hl)
	ld		(hl), a
	ld		e, #0

	// HL += BYTES_PER_LINE
	ld		a, #BYTES_PER_LINE
	add		a, l
	ld		l, a
	adc		a, h
	sub		a, l
	ld		h, a

_draw_vline_skip1:
	ex		af, af		// * A'
	sub		a, d
	jr		nc, _draw_vline_skip3

	// (d < 0)
	add		a, 5 (iy)	// A' += dy
	ex		af, af		// * A

	bit		7, 6 (iy)	// add_x
	jr		z, _draw_vline_skip4

	// (add_x < 0)
	ld		a, #0x0f
	and		a, c
	jr		z, _draw_vline_skip5

	ld		a, e
	or		a, (hl)
	ld		(hl), a
	ld		e, #0
	dec		hl

_draw_vline_skip6:
	// bit(C) <<= 4
	ld		a, c
	rlca
	rlca
	rlca
	rlca
	ld		c, a

	jr		_draw_vline_skip7

_draw_vline_skip4:
	// (add_x >= 0)
	ld		a, #0xf0
	and		a, c
	jr		z, _draw_vline_skip6

	ld		a, e
	or		a, (hl)
	ld		(hl), a
	ld		e, #0
	inc		hl

_draw_vline_skip5:
	// bit(C) >>= 4
	ld		a, c
	rrca
	rrca
	rrca
	rrca
	ld		c, a

	ex		af, af		// * A'

_draw_vline_skip3:
	ex		af, af		// * A

_draw_vline_skip7:
	// (d >= 0)
	ld		a, e
	or		a, c
	ld		e, a

	djnz	_draw_vline_loop1

_draw_vline_ret:
	ld		a, e
	or		a, a
	ret		z
	or		a, (hl)
	ld		(hl), a
	ret
__endasm;

#if 0
	uint8_t *vram;
	uint8_t bit;
	int8_t d;
	uint8_t num_y;
	uint8_t dest;

	vram = get_offscr_addr(((x0 & 0xfe) << 7) | (y0 >> 2));
	bit = 1 << (y0 & 3);
	if (x0 & 1) bit <<= 4;

	d = dy / 2;
	num_y = dy;

	dest = bit;
	while (num_y != 0) {
		if (bit & 0x88) {
			*vram |= dest;
			dest = 0;
			vram += BYTES_PER_LINE;
			bit >>= 3;
		} else {
			bit <<= 1;
		}

		d -= dx;
		if (d < 0) {
			d += dy;
			if (add_x < 0) {
				if (bit & 0x0f) {
					*vram |= dest;
					dest = 0;
					vram --;
					bit <<= 4;
				} else {
					bit >>= 4;
				}
			} else {
				if (bit & 0xf0) {
					*vram |= dest;
					dest = 0;
					vram ++;
					bit >>= 4;
				} else {
					bit <<= 4;
				}
			}
			dest |= bit;
		} else {
			dest |= bit;
		}
		num_y --;
	}
	if (dest != 0) *vram |= dest;
#endif
}
#endif

#if 0
//=============================================================================
void draw_vline(uint8_t x0, uint8_t y0, uint8_t dx, uint8_t dy, int8_t add_x)
{
#if 0
	uint8_t *vram;
	uint8_t bit;
	int8_t d;

	vram = get_offscr_addr(((x0 & 0xfe) << 7) | (y0 >> 2));
	bit = 1 << (y0 & 3);
	if (x0 & 1) {
		bit <<= 4;
	}

	d = (dy / 2) - dx;
	*vram |= bit;
	if (bit & 0x88) {
		vram += 120;
		bit >>= 3;
	} else {
		bit <<= 1;
	}
	num_y --;
	while (num_y != 0) {
		if (d < 0) {
			if (add_x < 0) {
				if (bit & 0x0f) {
					vram --;
					bit <<= 4;
				} else {
					bit >>= 4;
				}
			} else {
				if (bit & 0xf0) {
					vram ++;
					bit >>= 4;
				} else {
					bit <<= 4;
				}
			}
			*vram |= bit;
			d -= dx - dy;
		} else {
			*vram |= bit;
			d -= dx;
		}
		if (bit & 0x88) {
			vram += 120;
			bit >>= 3;
		} else {
			bit <<= 1;
		}
		num_y --;
	}
#endif

	x0; y0; dx; dy; add_x;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 3 (iy)
	ld		h, 2 (iy)

	call	_get_vram_bit

	ld		b, 5 (iy)
	inc		b
	ld		de, #BYTES_PER_LINE

	exx
	ld		l, 4 (iy)	// L = dx
	ld		h, 5 (iy)	// H = dy
	ld		a, h
	srl		a
	sub		a, l
	ld		e, a		// E = (dy / 2) - dx
	ld		a, l
	sub		a, h
	ld		h, a		// H = dx - dy
	ld		d, 6 (iy)	// D = add_x
	exx

_draw_vline_loop:
	ld		a, (hl)
	or		a, c
	ld		(hl), a

	call	_next_bity

	dec		b
	ret		z

	exx
	bit		7, e
	jr		z, _draw_vline_main1

	// case (d < 0)
	ld		a, e
	sub		a, h
	ld		e, a

	bit		7, d
	exx
	jr		z, _draw_vline_main2

	// case (add_x < 0)
	ld		a, c
	and		a, #0x0f
	jr		z, _draw_vline_main3
	dec		hl
_draw_vline_main4:
	ld		a, c
	add		a, a
	add		a, a
	add		a, a
	add		a, a
	ld		c, a
	jr		_draw_vline_loop
_draw_vline_main3:
	ld		a, c
	rrca
	rrca
	rrca
	rrca
	ld		c, a
	jr		_draw_vline_loop

_draw_vline_main2:
	// case (add_x > 0)
	ld		a, c
	and		a, #0xf0
	jr		z, _draw_vline_main4
	inc		hl
	jr		_draw_vline_main3

_draw_vline_main1:
	// case (d >= 0)
	ld		a, e
	sub		a, l
	ld		e, a
	exx
	jr		_draw_vline_loop
__endasm;
}
#endif

//=============================================================================
void draw_fast_hline(uint8_t x, uint8_t y, uint8_t num_x) __naked
{
	x; y; num_x;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 3 (iy)
	ld		h, 2 (iy)

	call	_get_vram_bity

	ld		d, 4 (iy)
	inc		d

	bit		0, 2 (iy)
	jr		z, _draw_fast_hline_skip1

	ld		a, b
	or		a, (hl)
	ld		(hl), a
	inc		hl
	dec		d

_draw_fast_hline_loop1:
#ifdef COUNT_VRTC_IN_LIB
	jp		z, _count_vrtc
#else
	ret		z
#endif

_draw_fast_hline_skip1:
	dec		d
	jr		z, _draw_fast_hline_skip2

	ld		a, b 
	or		a, c
	or		a, (hl)
	ld		(hl), a
	inc		hl

	dec		d
	jr		_draw_fast_hline_loop1

_draw_fast_hline_skip2:
	ld		a, c
	or		a, (hl)
	ld		(hl), a

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif

#if 0
	call	_get_vram_bit

	ld		b, 4 (iy)
	inc		b

_draw_fast_hline_loop:
	ld		a, (hl)
	or		a, c
	ld		(hl), a

	call	_next_bitx

	djnz	_draw_fast_hline_loop

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif
#endif
__endasm;
}

//=============================================================================
void draw_fast_vline(uint8_t x, uint8_t y, uint8_t num_y) __naked
{
	x; y; num_y;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 3 (iy)
	ld		h, 2 (iy)

	call	_get_vram_bit

	ld		de, #BYTES_PER_LINE

	ld		b, 4 (iy)
	inc		b

_draw_fast_vline_loop:
	ld		a, (hl)
	or		a, c
	ld		(hl), a

	call	_next_bity

	djnz	_draw_fast_vline_loop

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif
__endasm;
}

//=============================================================================
void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	x0; y0; x1; y1;
__asm
#ifdef REC_NUM_LINES
	ld		hl, #0x0102
	push	hl
	ld		hl, #_num_lines
	push	hl
	call	_bcd_add8
	pop		af
	pop		af
#endif

	ld		iy, #0
	add		iy, sp

	ld		a, 2 (iy)
	cp		#160
	ret		nc
	ld		c, a

	ld		a, 4 (iy)
	cp		#160
	ret		nc

	//sub		a, 2 (iy)
	sub		a, c
	ld		c, a		// C = x1 - x0

	ld		a, 3 (iy)
	cp		#100
	ret		nc
	ld		b, a

	ld		a, 5 (iy)
	cp		#100
	ret		nc

	//sub		a, 3 (iy)
	sub		a, b
	ld		b, a		// B = y1 - y0

	// C = dx, B = dy

	or		a, c
	ret		z

	ld		a, b
	or		a, a
	jr		nz, _draw_line_skip1

	// case (dy == 0)
	bit		7, c
	jr		z, _draw_line_hline1

	// case (add_x < 0) (dx < 0)
	xor		a, a
	sub		a, c
	ld		c, a
#if 1
	pop		hl	// ret addr
	pop		af	// x0y0
	pop		de	// x1y1

	push	bc
	push	de
	push	hl
	jp		_draw_fast_hline
#else
	ld		l, 4 (iy)
	ld		h, 5 (iy)
	push	bc
	push	hl
	call	_draw_fast_hline
	pop		af
	pop		af
	ret
#endif

_draw_line_hline1:
	// case (add_x > 0) (dx > 0)
#if 1
	pop		hl	// ret addr
	pop		de	// x0y0
	pop		af	// x1y1

	push	bc
	push	de
	push	hl
	jp		_draw_fast_hline
#else
	ld		l, 2 (iy)
	ld		h, 3 (iy)
	push	bc
	push	hl
	call	_draw_fast_hline
	pop		af
	pop		af
	ret
#endif

_draw_line_skip1:

	ld		a, c
	or		a, a
	jr		nz, _draw_line_skip2

	// case (dx == 0)
	bit		7, b
	jr		z, _draw_line_vline1

	// case (add_y < 0) (dy < 0)
	xor		a, a
	sub		a, b
	ld		c, a
#if 1
	pop		hl	// ret addr
	pop		af	// x0y0
	pop		de	// x1y1

	push	bc
	push	de
	push	hl
	jp		_draw_fast_vline
#else
	ld		l, 4 (iy)
	ld		h, 5 (iy)
	push	bc
	push	hl
	call	_draw_fast_vline
	pop		af
	pop		af
	ret
#endif

_draw_line_vline1:
	// case (add_y > 0) (dy > 0)
	ld		c, b
#if 1
	pop		hl	// ret addr
	pop		de	// x0y0
	pop		af	// x1y1

	push	bc
	push	de
	push	hl
	jp		_draw_fast_vline
#else
	ld		l, 2 (iy)
	ld		h, 3 (iy)
	push	bc
	push	hl
	call	_draw_fast_vline
	pop		af
	pop		af
	ret
#endif

_draw_line_skip2:

	ld		de, #0
	bit		7, c
	jr		z, _draw_line_skip3
	xor		a, a
	sub		a, c
	ld		c, a
	dec		e

_draw_line_skip3:

	bit		7, b
	jr		z, _draw_line_skip4
	xor		a, a
	sub		a, b
	ld		b, a
	dec		d

	// C = abs(dx), B = abs(dy)
	// E = add_x, D = add_y

_draw_line_skip4:

	ld		a, b
	cp		c
	jr		nc, _draw_line_vline2

	// case (dy < dx)
	// use draw_hline
	ld		a, d		// A = add_y
	bit		7, e
	jr		z, _draw_line_hline2
	// case (add_x < 0)
	cpl
	ld		l, 4 (iy)
	ld		h, 5 (iy)
	jr		_draw_line_hline3

_draw_line_hline2:
	// case (add_x > 0)
	ld		l, 2 (iy)
	ld		h, 3 (iy)

_draw_line_hline3:
#if 0
	push	af
	inc		sp
	push	bc
	push	hl
	call	_draw_hline
	pop		af
	pop		af
	inc		sp

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif

#else
#ifdef COUNT_VRTC_IN_LIB
	ld		de, #_count_vrtc
	push	de
#endif

	ex		de, hl
	ld		l, d
	ld		h, e
	bit		7, a
	jp		nz, _draw_hline_ym
	jp		_draw_hline_yp
#endif

_draw_line_vline2:
	// use draw_vline
	ld		a, e
	bit		7, d
	jr		z, _draw_line_vline3
	// case (add_y < 0)
	cpl
	ld		l, 4 (iy)
	ld		h, 5 (iy)
	jr		_draw_line_vline4

_draw_line_vline3:
	// case (add_y > 0)
	ld		l, 2 (iy)
	ld		h, 3 (iy)

_draw_line_vline4:
	//push	af
	//inc		sp
	push	bc
	push	hl
	bit		7, a
	jr		z, _draw_line_vline4_1
	call	_draw_vline_xm
	jr		_draw_line_vline4_2
_draw_line_vline4_1:
	call	_draw_vline_xp
_draw_line_vline4_2:
	//call	_draw_vline
	pop		af
	pop		af
	//inc		sp

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif
__endasm;

#if 0
	uint8_t dx, dy;
	int8_t add_x, add_y;

	if (x1 > x0) {
		dx = x1 - x0;
		add_x = 1;
	} else {
		dx = x0 - x1;
		add_x = -1;
	}
	if (y1 > y0) {
		dy = y1 - y0;
		add_y = 1;
	} else {
		dy = y0 - y1;
		add_y = -1;
	}
	if (dx == 0 && dy == 0) return;
	if (dy == 0) {
		if (add_x < 0) {
			draw_fast_hline(x1, y1, dx);
		} else {
			draw_fast_hline(x0, y0, dx);
		}
		return;
	} else if (dx == 0) {
		if (add_y < 0) {
			draw_fast_vline(x1, y1, dy);
		} else {
			draw_fast_vline(x0, y0, dy);
		}
		return;
	}
	if (dx >= 160 || dy >= 100) return;

	if (dy < dx) {
		if (add_x < 0) {
			draw_hline(x1, y1, dx, dy, -add_y);
		} else {
			draw_hline(x0, y0, dx, dy, add_y);
		}
	} else {
		if (add_y < 0) {
			draw_vline(x1, y1, dx, dy, -add_x);
		} else {
			draw_vline(x0, y0, dx, dy, add_x);
		}
	}
#endif
}

//=============================================================================
const Point_t *draw_lines(const Point_t *pts, uint8_t num_pts) __naked
{
	pts; num_pts;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)	// HL = path

	ld		c, (hl)
	inc		hl
	ld		b, (hl)		// BC = (x0, y0)
	inc		hl

	ld		a, 4 (iy)	// A = num_pts
	dec		a
	jr		nz, _draw_lines_skip1

	push	hl
	push	bc
#if 0
	ld		hl, (_draw_func)
	call	___sdcc_call_hl
#else
	call	_draw_pixel
#endif
	pop		af
	pop		hl
	ret

_draw_lines_skip1:
_draw_lines_loop:
	ld		e, (hl)
	inc		hl
	ld		d, (hl)		// DE = (x1, y1)
	inc		hl

	push	af
	push	hl
#if 0	// compare prev and cur, skip if same
	ld		a, c
	cp		e
	jr		nz, _draw_lines_skip2
	ld		a, b
	cp		d
	jr		nz, _draw_lines_skip2

	pop		hl
	pop		af
	dec		a
	jr		nz, _draw_lines_loop
	ret

_draw_lines_skip2:
#endif
	push	de		// push (x1, y1)

	push	de
	push	bc
	call	_draw_line
	pop		af
	pop		af

	pop		bc		// BC = (x1, y1)

	pop		hl
	pop		af
	dec		a
	jr		nz, _draw_lines_loop
	ret
__endasm;

#if 0
	const uint8_t *ptr;
	uint8_t x0, y0, x1, y1;
	uint8_t cnt;

	ptr = path;
	x0 = *ptr ++;
	y0 = *ptr ++;
	if (num_pts == 1) {
		*draw_func(x0, y0);
		return;
	}
	for (cnt = 0; cnt < num_pts - 1; cnt ++) {
		x1 = *ptr ++;
		y1 = *ptr ++;
		if (x0==x1 && y0==y1) continue;
		draw_line(x0, y0, x1, y1);
		x0 = x1;
		y0 = y1;
	}
#endif
}

//=============================================================================
void draw_path(const Point_t *pts_list, const uint8_t *num_pts)
{
	pts_list; num_pts;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)	// HL = pts_list
	ld		e, 4 (iy)
	ld		d, 5 (iy)	// DE = num_pts

_draw_path_loop:
	ld		a, (de)
	or		a, a
	ret		z

	push	de

	push	af
	inc		sp
	push	hl
	call	_draw_lines
	pop		af
	inc		sp

	pop		de
	inc		de
	jr		_draw_path_loop
__endasm;

#if 0
	while (*num_pts != 0) {
		pts_list = draw_lines(pts_list, *num_pts);
		num_pts ++;
	}
#endif
}

#if 0
//=============================================================================
void draw_path_list(const PathList_t *path_list)
{
#ifdef REC_NUM_LINES
	num_lines[0] = num_lines[1] = 0;
#endif

	draw_func = (draw_pixel_t *)draw_pixel;

	while (path_list->path != NULL) {
		draw_path(path_list->path, path_list->num_pts);

		path_list ++;
	}

#ifdef REC_NUM_LINES
	bcd_to_ascii(num_lines, 2, get_offscr_addr(0));
	*get_offscr_addr(0x5100) = 0xe8;
#endif
}
#endif

//=============================================================================
void set_line_func(uint8_t draw) __z88dk_fastcall __preserves_regs(b, c, d, e, iyh, iyl) __naked
{
	draw;
__asm
	ld		a, l
	or		a, a
	jr		z, _set_line_func_skip1
	ld		hl, #_draw_pixel
	ld		(_draw_func), hl
	ret
_set_line_func_skip1:
	ld		hl, #_clear_pixel
	ld		(_draw_func), hl
	ret
__endasm;
}

//=============================================================================
void init_line()
{
	draw_func = (draw_pixel_t *)draw_pixel;
}
