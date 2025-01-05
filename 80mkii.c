#include "80mkii.h"

//=============================================================================
//uint8_t port31h;

//=============================================================================
#if 0
void code_vram80() __naked
{
__asm
	.area		_EXTCODE (ABS)
	.org		0xc000
__endasm;
}
#endif

//=============================================================================
void n80_waitVBlank() __preserves_regs(h, l, d, e, b, c, iyh, iyl) __naked
{
__asm
__waitVBlank_0:
	in		a, (0x40)
	and		a, #0x20
	jr		nz, __waitVBlank_0
__waitVBlank_1:
	in		a, (0x40)
	and		a, #0x20
	jr		z, __waitVBlank_1
	ret
__endasm;
}

//=============================================================================
// Port 0x31
//   d7-d5 : Background Color (GRB)
//      d4 : 1=320x200, 0=640x200
//      d3 : 1=display on, 0=display off
//      d2 : when 320x200, 1=mode1, 0=mode0, when 640x200, 1=monochro, 0=attr
//      d1 : 1=RAM mode, 0=ROM mode (0x0000-0x7fff)
//      d0 : 1=N80 ROM, 0=EXT ROM (0x6000-0x7fff)
void n80_init_vram() __preserves_regs(h, l, d, e, b, c, iyh, iyl) __naked
{
__asm
	call	_n80_waitVBlank

	ld		a, #0b00111001
	out		(0x31), a
	ld		(_port31h), a

	ret

_port31h:
	.ds		1
__endasm;
}

//=============================================================================
void n80_set_bgcol(uint8_t col) __z88dk_fastcall __preserves_regs(d, e, b, c, iyh, iyl) __naked
{
	col;
__asm
	push	hl
	call	_n80_waitVBlank
	pop		hl

	ld		a, l
	rrca
	rrca
	rrca
	and		a, #0xe0
	ld		l, a
	ld		a, (_port31h)
	and		a, #0x1f
	or		a, l

	out		(0x31), a
	ld		(_port31h), a

	ret
__endasm;
}

//=============================================================================
void n80_enter_vram() __preserves_regs(h, l, d, e, b, c, iyh, iyl) __naked
{
__asm
	di
	xor		a
	out		(0x5c), a
	ret
__endasm;
}

//=============================================================================
void n80_exit_vram() __preserves_regs(h, l, d, e, b, c, iyh, iyl) __naked
{
__asm
	xor		a
	out		(0x5f), a
	ei
	ret
__endasm;
}

//=============================================================================
uint8_t *n80_vram_addr(uint16_t xy) __z88dk_fastcall __preserves_regs(iyh, iyl) __naked
{
	xy;
__asm
	// 0x8000 + y * 80 + x
	// h = x, l = y
	ld		e, h
	ld		d, #0x80	// DE = 0x8000 + x
	ld		h, #0
	ld		c, l
	ld		b, h
	add		hl, hl
	add		hl, hl
	add		hl, bc		// HL = L * 5
	add		hl, hl
	add		hl, hl
	add		hl, hl
	add		hl, hl
	add		hl, de
	ret
__endasm;
}

//=============================================================================
void n80_draw_bitmap(uint8_t x, uint8_t y, const Bitmap_t *bmp) __naked
{
	x; y; bmp;
__asm
	ld		iy, #0
	add		iy, sp

	ld		h, 2 (iy)	// H = x
	ld		l, 3 (iy)	// L = y
	call	_n80_vram_addr
	ex		de, hl		// DE = vram

	ld		l, 4 (iy)
	ld		h, 5 (iy)	// HL = bmp

	ld		c, (hl)		// C = bmp->w
	inc		hl
	ld		b, (hl)		// B = bmp->h
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a		// HL = bmp->data

	call	_n80_enter_vram

	ld		a, #80
	sub		a, c		// A = 80 - bmp->w

_n80_draw_bitmap_loop:
	push	bc
	ld		b, #0
	ldir
	ld		c, a
	ex		de, hl
	add		hl, bc
	ex		de, hl
	pop		bc
	djnz	_n80_draw_bitmap_loop

	call	_n80_exit_vram

	ret
__endasm;
#if 0
	uint8_t *vram;
	const uint8_t *src;
	uint8_t _x, _y, rem;

	n80_enter_vram();

	vram = n80_vram_addr(y | (x << 8));

	rem = 80 - bmp->w;
	src = bmp->data;
	for (_y = bmp->h; _y > 0; _y --) {
		for (_x = bmp->w; _x > 0; _x --) {
			*vram ++ = *src ++;
		}
		vram += rem;
	}

	n80_exit_vram();
#endif
}

//=============================================================================
void n80_draw_hline(uint8_t x, uint8_t y, uint8_t w, uint8_t pat) __naked
{
	x; y; w; pat;
__asm
	ld		iy, #0
	add		iy, sp

	call	_n80_enter_vram

	ld		l, 3 (iy)
	ld		h, 2 (iy)
	call	_n80_vram_addr

	ld		a, 5 (iy)
	ld		(hl), a
	ld		e, l
	ld		d, h
	inc		de

	ld		c, 4 (iy)
	ld		b, #0
	dec		c

	ldir

	jp		_n80_exit_vram
__endasm;

#if 0
	uint8_t *vram;

	n80_enter_vram();

	vram = n80_vram_addr(y | (x << 8));

	for (; w > 0; w --) {
		*vram ++ = pat;
	}

	n80_exit_vram();
#endif
}

//=============================================================================
void n80_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t pat) __naked
{
	x; y; w; h; pat;
__asm
	call	_n80_enter_vram

	ld		hl, #7
	add		hl, sp

	ld		d, (hl)
	dec		hl
	ld		e, (hl)		// DE = pat
	dec		hl
	ld		b, (hl)		// B = h
	dec		hl
	ld		c, (hl)		// C = w
	dec		hl
	ld		a, (hl)
	dec		hl
	ld		h, (hl)
	ld		l, a		// H = x, L = y

	push	bc
	push	de
	call	_n80_vram_addr
	pop		de
	pop		bc

	dec		c
	ld		a, d
	ex		af, af
	ld		a, e

_n80_fill_rect_loop:
	push	bc
	push	hl
	ld		b, #0
	ld		(hl), a
	ld		e, l
	ld		d, h
	inc		de
	ldir
	pop		hl
	ld		bc, #80
	add		hl, bc
	pop		bc
	ex		af, af
	djnz	_n80_fill_rect_loop

	jp		_n80_exit_vram
__endasm;
}

//=============================================================================
void n80_fill_rect32(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t pat) __naked
{
	x; y; w; h; pat;
__asm
	call	_n80_enter_vram

	ld		hl, #2
	add		hl, sp

	ld		d, (hl)		// D = x
	inc		hl
	ld		e, (hl)		// E = y
	inc		hl
	ld		c, (hl)		// C = w
	inc		hl
	ld		b, (hl)		// B = h
	inc		hl
	ex		de, hl		// H = x, L = y, DE = SP+6

	push	bc
	push	de
	call	_n80_vram_addr
	pop		de
	pop		bc

	ld		a, #4
	ex		af, af

	dec		c

_n80_fill_rect32_loop:
	ld		a, (de)
	inc		de
	push	de

	push	bc
	push	hl

	ld		b, #0
	ld		(hl), a
	ld		e, l
	ld		d, h
	inc		de
	ldir

	pop		hl
	ld		bc, #80
	add		hl, bc
	pop		bc

	pop		de
	ex		af, af
	dec		a
	jr		nz, _n80_fill_rect32_skip1

	ld		a, #4
	dec		de
	dec		de
	dec		de
	dec		de

_n80_fill_rect32_skip1:
	ex		af, af

	djnz	_n80_fill_rect32_loop

	jp		_n80_exit_vram
__endasm;
}
