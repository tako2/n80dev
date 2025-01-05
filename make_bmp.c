#include "n80dev.h"

//=============================================================================
static void copy_1bit_right(const uint8_t *src, uint8_t *dest, uint8_t w, uint8_t h) __naked
{
	src; dest; w; h;
__asm
	ld		hl, #7
	add		hl, sp
	ld		b, (hl)		// B = h
	dec		hl
	ld		c, (hl)		// C = w
	dec		hl
	ld		d, (hl)
	dec		hl
	ld		e, (hl)		// DE = dest
	dec		hl
	ld		a, (hl)
	dec		hl
	ld		l, (hl)
	ld		h, a		// HL = src

_copy_1bit_right_loop1:
	push	bc
	ld		b, c
	ld		c, #0

_copy_1bit_right_loop2:
	ld		a, (hl)
	rrca
	rrca
	rrca
	rrca
	and		a, #0x0f
	ex		af, af
	ld		a, (hl)
	inc		hl
	rrca
	rrca
	rrca
	rrca
	and		a, #0xf0
	or		a, c
	ld		(de), a
	inc		de
	ex		af, af
	ld		c, a

	djnz	_copy_1bit_right_loop2
	pop		bc
	djnz	_copy_1bit_right_loop1

	ret
__endasm;
}

//=============================================================================
static void next_hlde() __naked
{
__asm
	//ld		a, #4
	push	af
	add		a, l
	ld		l, a
	adc		a, h
	sub		a, l
	ld		h, a		// HL += 4

	//ld		a, #4
	pop		af
	add		a, e
	ld		e, a
	adc		a, d
	sub		a, e
	ld		d, a		// DE += 4

	ret
__endasm;
}

//=============================================================================
static void shift_down1() __naked
{
__asm
	// HL = src, DE = dest, C = prev pat
	// C = next pat
	ld		a, (hl)
	rlca
	and		a, #0xee
	or		a, c
	ld		(de), a

	ld		a, (hl)
	rrca
	rrca
	rrca
	and		a, #0x11
	ld		c, a

	ret
__endasm;
}

//=============================================================================
static void shift_down2() __naked
{
__asm
	// HL = src, DE = dest, C = prev pat
	// C = next pat
	ld		a, (hl)
	rlca
	rlca
	and		a, #0xcc
	or		a, c
	ld		(de), a

	ld		a, (hl)
	rrca
	rrca
	and		a, #0x33
	ld		c, a

	ret
__endasm;
}

//=============================================================================
static void shift_down3() __naked
{
__asm
	// HL = src, DE = dest, C = prev pat
	// C = next pat
	ld		a, (hl)
	rlca
	rlca
	rlca
	and		a, #0x88
	or		a, c
	ld		(de), a

	ld		a, (hl)
	rrca
	and		a, #0x77
	ld		c, a

	ret
__endasm;
}

//=============================================================================
static void copy_1bit_down(const uint8_t *src, uint8_t *dest, uint8_t w, uint8_t h, uint8_t ext) __naked
{
	src; dest; w; h; ext;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)	// HL = src
	ld		e, 4 (iy)
	ld		d, 5 (iy)	// DE = dest
	ld		c, 6 (iy)	// C = w
	ld		b, 7 (iy)	// B = h

_copy_1bit_down_loop1:
	push	bc
	push	de
	push	hl

	ld		c, #0
_copy_1bit_down_loop2:
	call	_shift_down1
	ld		a, 6 (iy)
	call	_next_hlde
	djnz	_copy_1bit_down_loop2

	ld		a, 8 (iy)
	or		a, a
	jr		z, _copy_1bit_down_skip
	ex		de, hl
	ld		(hl), c

_copy_1bit_down_skip:
	pop		hl
	inc		hl
	pop		de
	inc		de
	pop		bc

	dec		c
	jr		nz, _copy_1bit_down_loop1

	ret
__endasm;
}

//=============================================================================
static void copy_2bit_down(const uint8_t *src, uint8_t *dest, uint8_t w, uint8_t h, uint8_t ext) __naked
{
	src; dest; w; h; ext;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)	// HL = src
	ld		e, 4 (iy)
	ld		d, 5 (iy)	// DE = dest
	ld		c, 6 (iy)	// C = w
	ld		b, 7 (iy)	// B = h

_copy_2bit_down_loop1:
	push	bc
	push	de
	push	hl

	ld		c, #0
_copy_2bit_down_loop2:
	call	_shift_down2
	ld		a, 6 (iy)
	call	_next_hlde
	djnz	_copy_2bit_down_loop2

	ld		a, 8 (iy)
	or		a, a
	jr		z, _copy_2bit_down_skip
	ex		de, hl
	ld		(hl), c

_copy_2bit_down_skip:
	pop		hl
	inc		hl
	pop		de
	inc		de
	pop		bc

	dec		c
	jr		nz, _copy_2bit_down_loop1

	ret
__endasm;
}

//=============================================================================
static void copy_3bit_down(const uint8_t *src, uint8_t *dest, uint8_t w, uint8_t h, uint8_t ext) __naked
{
	src; dest; w; h; ext;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)	// HL = src
	ld		e, 4 (iy)
	ld		d, 5 (iy)	// DE = dest
	ld		c, 6 (iy)	// C = w
	ld		b, 7 (iy)	// B = h

_copy_3bit_down_loop1:
	push	bc
	push	de
	push	hl

	ld		c, #0
_copy_3bit_down_loop2:
	call	_shift_down3
	ld		a, 6 (iy)
	call	_next_hlde
	djnz	_copy_3bit_down_loop2

	ld		a, 8 (iy)
	or		a, a
	jr		z, _copy_3bit_down_skip
	ex		de, hl
	ld		(hl), c

_copy_3bit_down_skip:
	pop		hl
	inc		hl
	pop		de
	inc		de
	pop		bc

	dec		c
	jr		nz, _copy_3bit_down_loop1

	ret
__endasm;
}

//=============================================================================
void make_1px_bmps(const uint8_t *src, Bitmap_t *bmp, uint8_t *work, uint8_t w, uint8_t h, uint8_t size, uint8_t ext)
{
	uint8_t new_h, new_size;

	bmp->w = w;
	bmp->h = h;
	bmp->data = src;
	bmp ++;

	copy_1bit_right(src, work, w, h);
	bmp->w = w;
	bmp->h = h;
	bmp->data = work;
	work += size;
	bmp ++;

	// 1-bit down
	new_h = h;
	new_size = size;
	if (ext >= 3) {
		new_h ++;
		new_size += w;
		copy_1bit_down(src, work, w, h, 1);
	} else {
		copy_1bit_down(src, work, w, h, 0);
	}
	bmp->w = w;
	bmp->h = new_h;
	bmp->data = work;
	work += new_size;
	bmp ++;

	copy_1bit_right(work-new_size, work, w, new_h);
	bmp->w = w;
	bmp->h = new_h;
	bmp->data = work;
	work += new_size;
	bmp ++;

	// 2-bit down
	new_h = h;
	new_size = size;
	if (ext >= 2) {
		new_h ++;
		new_size += w;
		copy_2bit_down(src, work, w, h, 1);
	} else {
		copy_2bit_down(src, work, w, h, 0);
	}
	bmp->w = w;
	bmp->h = new_h;
	bmp->data = work;
	work += new_size;
	bmp ++;

	copy_1bit_right(work-new_size, work, w, new_h);
	bmp->w = w;
	bmp->h = new_h;
	bmp->data = work;
	work += new_size;
	bmp ++;

	// 3-bit down
	new_h = h;
	new_size = size;
	if (ext >= 1) {
		new_h ++;
		new_size += w;
		copy_3bit_down(src, work, w, h, 1);
	} else {
		copy_3bit_down(src, work, w, h, 0);
	}
	bmp->w = w;
	bmp->h = new_h;
	bmp->data = work;
	work += new_size;
	bmp ++;

	copy_1bit_right(work-new_size, work, w, new_h);
	bmp->w = w;
	bmp->h = new_h;
	bmp->data = work;
	work += new_size;
	bmp ++;
}

#if 0
//=============================================================================
void make_bmps(const uint8_t src, Bitmap_t *bmp, uint8_t num, uint8_t w, uint8_t h, uint8_t size)
{
	for (; num != 0; num --) {
		bmp->w = w;
		bmp->h = h;
		bmp->data = src;
		src += size;
	}
}
#endif
