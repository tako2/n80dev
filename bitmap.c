#include "n80dev.h"

//#define NOT_USE_CLIPPING

//#define BMP_BLIT_MASK
//#define BMP_BLIT_OR
//#define BMP_BLIT_XOR

#ifdef BMP_ENABLE_BLIT_OP
uint8_t blit_op;
#endif

//=============================================================================
#if 0 //def BMP_ENABLE_BLIT_OP
void set_blit_op(uint8_t op) __z88dk_fastcall
{
	blit_op = op;
}
#endif

//=============================================================================
void clip_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	x; y; w; h;
#ifdef NOT_USE_CLIPPING
__asm
	ret
__endasm;
#else
__asm
	ld		hl, #2
	add		hl, sp

	ld		a, (hl)		// A = x
	ld		(_clip_hline_x0+1), a

	inc		hl
	inc		hl
	ld		a, (hl)		// A = w
	inc		a
	ld		(_clip_hline_w0+1), a
__endasm;
#endif
}

#ifndef NOT_USE_CLIPPING
//=============================================================================
void clip_vline() __naked
{
__asm
	// input/output
	//   H  uint8_t y
	//   L  uint8_t height
	// output
	//   A  uint8_t src_h_ofs
	// not use
	//   BC, DE

	// collision scrY to +scrH
	// compare A and B
	// A = (y - (scrY+h-1)) + h - 1 = y - scrY
	// B = (scrH-(2h-2)) + h - 1 = scrH - 2h + 2 + h - 1 = scrH + 1 - h

	ld		a, h
#ifdef CLIP_VLINE_Y0
	dec		a
#endif
	add		a, l
	jr		c, _clip_vline_y_minus

#ifdef CLIP_VLINE_Y0
	sub		a, #24
#else
	sub		a, #26
#endif
	jr		nc, _clip_vline_need_clip
	xor		a, a
	ret
	// total = 43 clk + call(17 clk)

_clip_vline_need_clip:
	// clip bottom
	inc		a		// A = y + h - 25
	sub		a, l
	jr		nc, _clip_vline_clipped
	neg
	ld		l, a
	xor		a, a
	ret
	// 39+41 = 80 clk

_clip_vline_y_minus:
	jr		z, _clip_vline_clipped
	ld		l, a	// L = new height
#ifdef CLIP_VLINE_Y0
	ld		a, #1
	sub		a, h
	ld		h, #1
#else
	xor		a, a
	sub		a, h	// A = add_src (Y0 - y)
	ld		h, #0	// H = Y0
#endif
	ret
	// 20+36 = 56 clk

_clip_vline_clipped:
	// *** two-func ret, use prev-func ix ***
	ld		l, #0
	ld		sp, ix
	pop		ix
	ret
__endasm;
}
#endif

#ifndef NOT_USE_CLIPPING
//=============================================================================
void clip_hline() __naked
{
__asm
	// input/output
	// HL uint8_t *src
	// B  uint8_t x
	// C  uint8_t w

    // output
	// A  uint8_t skip_src

	// work
	// DE

_clip_hline_x0:
	ld		e, #0		// X0
	ld		a, b
	sub		a, e
	add		a, c
	jr		c, _clip_hline_left
_clip_hline_w0:
	sub		a, #81		// W0 + 1
	jr		nc, _clip_hline_right
	xor		a, a
	ret
	// total = 54 clk + call(17 clk)

_clip_hline_left:
	jr		z, _clip_hline_clipped
	ld		c, a		// C = new width
	ld		a, e
	sub		a, b		// A = skip_src
	ld		b, e		// B = X0
	ld		e, a
	ld		d, #0
	add		hl, de		// HL += skip_src
	ret

_clip_hline_right:
	inc		a			// A = (x - X0) + w - W0
	cp		c
	jr		nc, _clip_hline_clipped
	ld		e, a
	ld		a, c
	sub		a, e
	ld		c, a
	ld		a, e
	ret

_clip_hline_clipped:
	// *** two-func ret, use prev-func ix ***
	ld		l, #0
	ld		sp, ix
	pop		ix
	ret
__endasm;
}
#endif

//=============================================================================
#ifdef BMP_USE_OR_BITMAP
uint8_t or_bitmap(uint8_t x, uint8_t y, const Bitmap_t *bitmap, uint8_t color) __naked
{
	x; y; bitmap; color;
__asm
	pop		hl
	ld		(_or_bitmap_ret+1), hl

	ld		a, #BLIT_OP_OR
	ld		(_blit_op), a

	call	_put_bitmap

	xor		a, a
	ld		(_blit_op), a

_or_bitmap_ret:
	jp		0
__endasm;
}
#endif

//=============================================================================
#ifdef BMP_USE_BITMAP3
uint8_t put_bitmap3(uint8_t x, uint8_t y, const Bitmap_t *bitmap, uint8_t color) __naked
{
	x; y; bitmap; color;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		l, 6 (ix)	// HL = _bitmap
	ld		h, 7 (ix)

	sra		4 (ix)		// x = x >> 1, CY = x[0]

	jr		nc, _put_bitmap3_skip1
	ld		bc, #04
	add		hl, bc

_put_bitmap3_skip1:
	xor		a, a
	sra		5 (ix)
	rra
	sra		5 (ix)		// y = y >> 2, A = y[1:0] * 8
	rra
	rra
	rra
	rra
	add		a, l
	ld		l, a
	adc		a, h
	sub		a, l
	ld		h, a

	jr		_put_bitmap2_hook
__endasm;
}
#endif

//=============================================================================
#ifdef BMP_USE_BITMAP2
uint8_t put_bitmap2(uint8_t x, uint8_t y, const Bitmap_t *bitmap, uint8_t color) __naked
{
	x; y; bitmap; color;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		l, 6 (ix)	// HL = _bitmap
	ld		h, 7 (ix)

	sra		5 (ix)		// y >> 1

	jr		nc, _put_bitmap2_hook
	ld		bc, #04
	add		hl, bc
	jr		_put_bitmap2_hook
__endasm;
}
#endif

//=============================================================================
uint8_t put_bitmap(uint8_t x, uint8_t y, const Bitmap_t *bitmap, uint8_t color) __naked
{
	x; y; bitmap; color;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		l, 6 (ix)	// HL = _bitmap
	ld		h, 7 (ix)

	//-------------------------------------------------------------------------
_put_bitmap2_hook:
	ld		c, (hl)		// C = _bitmap->w
	ld		b, 4 (ix)	// B = _x
	inc		hl
	ld		a, (hl)		// _bitmap->h
	inc		hl
	ld		e, (hl)		// HL = _bitmap=>data
	inc		hl
	ld		d, (hl)
	// B = x, C = w, DE = bmp_src, A = lines
	ld		l, a		// l = lines
	ld		h, 5 (ix)	// h = y
	// B = x, C = w, DE = bmp_src, L = lines, H = y

	//-------------------------------------------------------------------------
_put_bitmap_stub:
	// B = x, C = w, DE = bmp_src, L = lines, H = y

#ifdef NOT_USE_CLIPPING
	ex		de, hl	// HL = bmp_src, D = y, E = lines

	xor		a, a
	ex		af, af		// A' = skip_src

	ld		a, b
	ld		b, e
	push	bc		// store B = lines, C = width
	ld		b, a
#else
	//-------------------------------------------------------------------------
	// *** Clip Vertical ***
	call	_clip_vline
	// H = y, L = lines, A = add_src
	// already return when (L == 0)
	//ld		-1 (ix), l	// store lines
	push	hl			// *2 (y, lines)
	and		a, a
	jr		z, _put_bitmap_clip_hline

	// DE(bmp_src) += C(w) * A(add_src)
	// DE = 0 | a
	// HL = c | 0
	push	de
	push	bc
	ld		l, #0
	ld		h, a
	ld		e, c
	ld		d, l
	ld		b, #8
_put_bitmap_clip2:
	add		hl, hl
	jr		nc, _put_bitmap_clip3
	add		hl, de
_put_bitmap_clip3:
	djnz	_put_bitmap_clip2
	pop		bc
	pop		de

	add		hl, de
	ex		de, hl
	// DE = bmp_src, B = x, C = w

	//-------------------------------------------------------------------------
	// *** Clip Horizontal ***
_put_bitmap_clip_hline:
	ex		de, hl
	// HL = bmp_src, B = x, C = w, DE is work
	call	_clip_hline

	// HL = bmp_src, B = x, C = w, A = skip_src
	ex		af, af	// A' = skip_src

	pop		de		// *2 (y, lines)
	// HL = bmp_src, B = x, C = w, D = y, E = lines
#endif	// NOT_USE_CLIPPING

	ld		a, b
	ld		b, e	// B = lines
	ld		e, d	// E = y
	ld		d, a	// D = x
	// HL = bmp_src, B = lines, C = w, D = x, E = y

	push	bc		// store B = lines, C = width
	push	hl		// *1 (bmp_src)

	//-------------------------------------------------------------------------
	ld		a, 8 (ix)	// color
	and		a, a
	jr		z, _put_bitmap_skip
	// 130 clk

	push	de

	// as _fill_rect_attr
	ld		h, a
	ld		l, c

_put_bitmap_fill_attr:
	push	bc

	push	hl		// (attr, w)
	push	de		// (x, y)
_bitmap_insert_attr::
	call	_insert_attr
	pop		de		// WARNING (may not same)
	pop		hl		// WARNING (may not same)

	inc		e
	pop		bc
	djnz	_put_bitmap_fill_attr

	pop		de	// D = x, E = y

	//-------------------------------------------------------------------------
_put_bitmap_skip:
	ex		de, hl
	call	_get_offscr_addr	// 130 clk
	ex		de, hl
	// DE = bmp_dest

	pop		hl		// *1 (bmp_src)
	// DE = dest, HL = src

	pop		bc		// stored B = height, C = width

#ifdef BMP_ENABLE_BLIT_OP
	ld		a, (_blit_op)
	or		a, a
	jp		z, __blit_nop

	ex		de, hl

	dec		a
#ifdef BMP_BLIT_MASK
	jp		z, __blit_mask
#endif
	dec		a
#ifdef BMP_BLIT_OR
	jp		z, __blit_or
#endif
#ifdef BMP_BLIT_XOR
	jp		__blit_xor
#else
	ex		de, hl
	jp		__blit_nop
#endif
#else
	//jp		__blit_nop
	// _blit_nop() is plased next.
#endif
__endasm;
}

//=============================================================================
static void _blit_nop() __naked
{
__asm
	// HL = src
	// DE = dest
	// B = height, C = width
	// A' = skip_src
	ld		a, #BYTES_PER_LINE
	sub		a, c		// A = skip_dest

#ifndef NOT_USE_CLIPPING
	ex		af, af
	or		a, a
	jr		nz, __blit_nop_loop
	ex		af, af
#endif

__blit_nop_loop0:
	push	bc

	ld		b, #0
	ldir

	ld		c, a
	ex		de, hl
	add		hl, bc
	ex		de, hl

	pop		bc
	djnz	__blit_nop_loop0

__blit_ret:

#ifdef COUNT_VRTC_IN_LIB
	call	_count_vrtc
#endif

	pop		ix
	ld		l, #1
	ret

#ifndef NOT_USE_CLIPPING
__blit_nop_loop:
	push	bc
	ld		b, #0
	ldir

	ld		c, a
	add		hl, bc

	ex		de, hl
	ex		af, af
	ld		c, a
	add		hl, bc
	ex		af, af
	ex		de, hl

	pop		bc
	djnz	__blit_nop_loop

	jp		__blit_ret
#endif
__endasm;
}

#ifdef BMP_BLIT_MASK
//=============================================================================
static void _blit_mask() __naked
{
__asm
	// DE = src
	// HL = dest
	// B = height, C = width
	// A' = skip_src
	ld		a, #BYTES_PER_LINE
	sub		a, c		// A = skip_dest

__blit_mask_loop2:
	push	bc
	ld		b, c
	ld		c, a

__blit_mask_loop1:
	ld		a, (de)
	and		a, (hl)
	ld		(hl), a
	inc		de
	inc		hl
	djnz	__blit_mask_loop1

	add		hl, bc
	ld		a, c

#ifndef NOT_USE_CLIPPING
	ex		de, hl
	ex		af, af
	ld		c, a
	add		hl, bc
	ex		af, af
	ex		de, hl
#endif

	pop		bc
	djnz	__blit_mask_loop2

	jp		__blit_ret
__endasm;
}
#endif

#ifdef BMP_BLIT_OR
//=============================================================================
static void _blit_or() __naked
{
__asm
	// DE = src
	// HL = dest
	// B = height, C = width
	// A' = skip_src
	ld		a, #BYTES_PER_LINE
	sub		a, c		// A = skip_dest

__blit_or_loop2:
	push	bc
	ld		b, c
	ld		c, a

__blit_or_loop1:
	ld		a, (de)
	or		a, (hl)
	ld		(hl), a
	inc		de
	inc		hl
	djnz	__blit_or_loop1

	add		hl, bc
	ld		a, c

#ifndef NOT_USE_CLIPPING
	ex		de, hl
	ex		af, af
	ld		c, a
	add		hl, bc
	ex		af, af
	ex		de, hl
#endif

	pop		bc
	djnz	__blit_or_loop2

	jp		__blit_ret
__endasm;
}
#endif

#ifdef BMP_BLIT_XOR
//=============================================================================
static void _blit_xor() __naked
{
__asm
	// DE = src
	// HL = dest
	// B = height, C = width
	// A' = skip_src
	ld		a, #BYTES_PER_LINE
	sub		a, c		// A = skip_dest

__blit_xor_loop2:
	push	bc
	ld		b, c
	ld		c, a

__blit_xor_loop1:
	ld		a, (de)
	xor		a, (hl)
	ld		(hl), a
	inc		de
	inc		hl
	djnz	__blit_xor_loop1

	add		hl, bc
	ld		a, c

#ifndef NOT_USE_CLIPPING
	ex		de, hl
	ex		af, af
	ld		c, a
	add		hl, bc
	ex		af, af
	ex		de, hl
#endif

	pop		bc
	djnz	__blit_xor_loop2

	jp		__blit_ret
__endasm;
}
#endif

//=============================================================================
#ifdef BMP_USE_FIXED_BITMAP
uint8_t put_fixed_bitmap(uint8_t x, uint8_t y, const uint8_t *data, uint8_t color) __naked
{
	x; y; data; color;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		e, 6 (ix)
	ld		d, 7 (ix)	// DE = data

	ld		h, 5 (ix)
	sra		h        	// H = y >> 1
	jr		c, _put_fixed_bitmap_y0
	ld		l, #FIXED_BITMAP_H		// L = lines
	jr		_put_fixed_bitmap_y1
_put_fixed_bitmap_y0:
	ld		l, #FIXED_BITMAP_H+1	// L = lines

	ex		de, hl
	ld		bc, #FIXED_BITMAP_W * FIXED_BITMAP_H
	add		hl, bc
	ex		de, hl
_put_fixed_bitmap_y1:

	ld		c, #FIXED_BITMAP_W
	ld		b, 4 (ix)	// b = _x
	// B = x, C = w, DE = bmp_src, L = lines, H = y

	jp		_put_bitmap_stub
__endasm;
}
#endif

//=============================================================================
void put_bitmap_vram(/* uint8_t *pat, uint16_t hw, uint8_t *vram */)
{
__asm
	// HL = pat
	// DE = vram
	// B = height
	// C = width

	ld		a, #BYTES_PER_LINE
	sub		a, c		// A = 120 - width

_put_bitmap_vram_loop:
	push	bc
	ld		b, #0

	ldir

	ex		de, hl
	ld		c, a
	add		hl, bc
	ex		de, hl		// DE += (120 - width)

	pop		bc
	djnz	_put_bitmap_vram_loop

	ret
__endasm;
}
