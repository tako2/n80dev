#include "n80dev.h"

//=============================================================================
static uint8_t pixel_work[(40 + 1) * (25 + 1)];

extern uint8_t blit_op;

//=============================================================================
static uint16_t shift_y1(uint8_t bits) __z88dk_fastcall __naked
{
	bits;
__asm
	ld		a, l
	rlca
	and		a, #0xee
	ld		h, a
	ld		a, l
	rrca
	rrca
	rrca
	and		a, #0x11
	ld		l, a
	ret
__endasm;
}

//=============================================================================
static uint16_t shift_y2(uint8_t bits) __z88dk_fastcall __naked
{
	bits;
__asm
	ld		a, l
	rlca
	rlca
	and		a, #0xcc
	ld		h, a
	ld		a, l
	rrca
	rrca
	and		a, #0x33
	ld		l, a
	ret
__endasm;
}

//=============================================================================
static uint16_t shift_y3(uint8_t bits) __z88dk_fastcall __naked
{
	bits;
__asm
	ld		a, l
	rlca
	rlca
	rlca
	and		a, #0x88
	ld		h, a
	ld		a, l
	rrca
	and		a, #0x77
	ld		l, a
	ret
__endasm;
}

//static const uint8_t mask1[] = { 0, 0xee, 0xcc, 0x88 };
//static const uint8_t mask2[] = { 0, 0x11, 0x33, 0x77 };

//=============================================================================
typedef uint16_t shift_y_func_t (uint8_t bits) __z88dk_fastcall;
extern const void *shift_y_funcs[];
void shift_y_funcs_def()
{
__asm
_shift_y_funcs:
	.dw		0
	.dw		_shift_y1
	.dw		_shift_y2
	.dw		_shift_y3
__endasm;
}

//=============================================================================
static void shift_y(const uint8_t *src,
					uint8_t *dest1, uint8_t *dest2,
					uint8_t w, void *func) __naked
{
	src; dest1; dest2; w; func;
__asm
	ld		iy, #0
	add		iy, sp

	ld		e, 4 (iy)
	ld		d, 5 (iy)
	ld		c, 6 (iy)
	ld		b, 7 (iy)

	ld		l, 9 (iy)
	ld		h, 10 (iy)

	push	hl
	ld		l, 2 (iy)
	ld		h, 3 (iy)
	ld		a, 8 (iy)
	pop		iy

	// HL = src
	// DE = dest1
	// BC = dest2
	// A = w
	// IY = func

_shift_y_loop1:
	ex		af, af
	push	hl
	ld		a, (hl)
	ld		hl, #_shift_y_ret_addr
	push	hl
	ld		l, a
	jp		(iy)
_shift_y_ret_addr:
	ld		a, (de)
	or		a, h
	ld		(de), a
	inc		de
	ld		a, l
	ld		(bc), a
	inc		bc
	pop		hl
	inc		hl

	ex		af, af
	dec		a
	jr		nz, _shift_y_loop1

	ret
__endasm;

#if 0
	shift_y_func_t *func;
	func = shift_y_funcs[n];

	for (; w != 0; w --) {
#if 0
		*dest1 |= (*src << n) & mask1[n];
		*dest2 = (*src >> (4 - n)) & mask2[n];
#else
		uint16_t res;
		res = (*func)(*src);
		*dest1 |= res >> 8;
		*dest2 = res & 0xff;
#endif
		src ++;
		dest1 ++;
		dest2 ++;
	}
#endif
}

//=============================================================================
static void shift_x_same(uint8_t *src, uint8_t w, uint8_t h) __naked
{
	src; w; h;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)
	ld		b, 4 (iy)
	ld		c, 5 (iy)

	// HL = src
	// B = width, C = height

_shift_x_same_loop2:
	push	bc
	xor		a, a

_shift_x_same_loop1:
	rld
	inc		hl
	djnz	_shift_x_same_loop1

	ld		(hl), a
	inc		hl

	pop		bc
	dec		c
	jr		nz, _shift_x_same_loop2

	ret
__endasm;
}

//=============================================================================
static void shift_x_same_mask(uint8_t *src, uint8_t w, uint8_t h) __naked
{
	src; w; h;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)
	ld		b, 4 (iy)
	ld		c, 5 (iy)

	// HL = src
	// B = width, C = height

_shift_x_same_mask_loop2:
	push	bc
	ld		a, #0xff

_shift_x_same_mask_loop1:
	rld
	inc		hl
	djnz	_shift_x_same_mask_loop1

	ld		(hl), a
	inc		hl

	pop		bc
	dec		c
	jr		nz, _shift_x_same_mask_loop2

	ret
__endasm;
}

//=============================================================================
static void shift_x(uint8_t *src, uint8_t *dest, uint8_t w, uint8_t h) __naked
{
	src; dest; w; h;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)
	ld		e, 4 (iy)
	ld		d, 5 (iy)
	ld		b, 6 (iy)
	ld		c, 7 (iy)

	// HL = src
	// DE = dest
	// B = width, C = height

_shift_x_loop2:
	push	bc
	ld		c, #0

_shift_x_loop1:
	ld		a, (hl)
	rrca
	rrca
	rrca
	rrca

	push	af
	and		a, #0xf0
	or		a, c
	ld		(de), a

	pop		af
	and		a, #0x0f
	ld		c, a

	inc		hl
	inc		de

	djnz	_shift_x_loop1

	ld		a, c
	ld		(de), a
	inc		de

	pop		bc
	dec		c
	jr		nz, _shift_x_loop2

	ret
__endasm;

#if 0
	uint8_t prev, next;
	prev = 0;
	for (; w != 0; w --) {
		next = *src >> 4;
		*dest = prev | (*src << 4);
		prev = next;
		src ++;
		dest ++;
	}
	*dest = prev;
#endif
}

//=============================================================================
static void shift_x_mask(uint8_t *src, uint8_t *dest, uint8_t w, uint8_t h) __naked
{
	src; dest; w; h;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)
	ld		e, 4 (iy)
	ld		d, 5 (iy)
	ld		b, 6 (iy)
	ld		c, 7 (iy)

	// HL = src
	// DE = dest
	// B = width, C = height

_shift_x_mask_loop2:
	push	bc
	ld		c, #0xff

_shift_x_mask_loop1:
	ld		a, (hl)
	rrca
	rrca
	rrca
	rrca

	push	af
	or		a, #0x0f
	and		a, c
	ld		(de), a

	pop		af
	or		a, #0xf0
	ld		c, a

	inc		hl
	inc		de

	djnz	_shift_x_mask_loop1

	ld		a, c
	ld		(de), a
	inc		de

	pop		bc
	dec		c
	jr		nz, _shift_x_mask_loop2

	ret
__endasm;

#if 0
	uint8_t prev, next;
	prev = 0xff;
	for (; w != 0; w --) {
		next = (*src >> 4) | 0xf0;
		*dest = prev & ((*src << 4) | 0x0f);
		prev = next;
		src ++;
		dest ++;
	}
	*dest = prev;
#endif
}

//=============================================================================
static uint16_t shift_y1_mask(uint8_t bits) __z88dk_fastcall __naked
{
	bits;
__asm
	ld		a, l
	rlca
	or		a, #0x11
	ld		h, a
	ld		a, l
	rrca
	rrca
	rrca
	or		a, #0xee
	ld		l, a
	ret
__endasm;
}

//=============================================================================
static uint16_t shift_y2_mask(uint8_t bits) __z88dk_fastcall __naked
{
	bits;
__asm
	ld		a, l
	rlca
	rlca
	or		a, #0x33
	ld		h, a
	ld		a, l
	rrca
	rrca
	or		a, #0xcc
	ld		l, a
	ret
__endasm;
}

//=============================================================================
static uint16_t shift_y3_mask(uint8_t bits) __z88dk_fastcall __naked
{
	bits;
__asm
	ld		a, l
	rlca
	rlca
	rlca
	or		a, #0x77
	ld		h, a
	ld		a, l
	rrca
	or		a, #0x88
	ld		l, a
	ret
__endasm;
}

//=============================================================================
extern const void *shift_y_mask_funcs[];
void shift_y_mask_funcs_def()
{
__asm
_shift_y_mask_funcs:
	.dw		0
	.dw		_shift_y1_mask
	.dw		_shift_y2_mask
	.dw		_shift_y3_mask
__endasm;
}

//=============================================================================
static void shift_y_mask(const uint8_t *src,
						 uint8_t *dest1, uint8_t *dest2, uint8_t w,
						 void *func)
{
	src; dest1; dest2; w; func;
__asm
	ld		iy, #0
	add		iy, sp

	ld		e, 4 (iy)
	ld		d, 5 (iy)
	ld		c, 6 (iy)
	ld		b, 7 (iy)

	ld		l, 9 (iy)
	ld		h, 10 (iy)

	push	hl
	ld		l, 2 (iy)
	ld		h, 3 (iy)
	ld		a, 8 (iy)
	pop		iy

	// HL = src
	// DE = dest1
	// BC = dest2
	// A = w
	// IY = func

_shift_y_mask_loop1:
	ex		af, af
	push	hl
	ld		a, (hl)
	ld		hl, #_shift_y_mask_ret_addr
	push	hl
	ld		l, a
	jp		(iy)
_shift_y_mask_ret_addr:
	ld		a, (de)
	and		a, h
	ld		(de), a
	inc		de
	ld		a, l
	ld		(bc), a
	inc		bc
	pop		hl
	inc		hl

	ex		af, af
	dec		a
	jr		nz, _shift_y_mask_loop1

	ret
__endasm;


#if 0
	shift_y_func_t *func;
	func = shift_y_mask_funcs[n];

	for (; w != 0; w --) {
#if 0
		*dest1 &= (*src << n) | mask2[n];
		*dest2 = (*src >> (4 - n)) | mask1[n];
#else
		uint16_t res;
		res = (*func)(*src);
		*dest1 &= res >> 8;
		*dest2 = res & 0xff;
#endif
		src ++;
		dest1 ++;
		dest2 ++;
	}
#endif
}

//=============================================================================
static void memset(uint8_t *ptr, uint8_t size, uint8_t data) __naked
{
	ptr; size; data;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)
	ld		b, #0
	ld		c, 4 (iy)
	ld		a, 5 (iy)

	ld		(hl), a
	ld		e, l
	ld		d, h
	inc		de
	dec		c
	ret		z

	ldir

	ret
__endasm;
}

//=============================================================================
const Bitmap_t shift_bmp = { 0, 0, pixel_work };
Bitmap_t *shift_bitmap(uint8_t bx, uint8_t by, Bitmap_t *ptr)
{
	const uint8_t *src;
	uint8_t *dest;
	uint8_t cnt;
	Bitmap_t *bmp;
	//shift_y_func_t *func;
	void *func;

	bmp = (Bitmap_t *)shift_bmp;
	bmp->w = ptr->w + bx;
	bmp->h = ptr->h + (by != 0);

	src = ptr->data;
	dest = pixel_work;
	if (by != 0) {
		if (blit_op == BLIT_OP_MASK) {
			memset(dest, ptr->w, 0xff);
			func = (void*)shift_y_mask_funcs[by];
			for (cnt = ptr->h; cnt != 0; cnt --) {
				shift_y_mask(src, dest, dest + bmp->w, ptr->w, (void*)func);
				src += ptr->w;
				dest += bmp->w;
			}
			if (bx != 0) {
				shift_x_same_mask(pixel_work, ptr->w, bmp->h);
			}
		}
		else {
			memset(dest, ptr->w, 0x00);
			func = (void*)shift_y_funcs[by];
			for (cnt = ptr->h; cnt != 0; cnt --) {
				shift_y(src, dest, dest + bmp->w, ptr->w, (void*)func);
				src += ptr->w;
				dest += bmp->w;
			}
			if (bx != 0) {
				shift_x_same(pixel_work, ptr->w, bmp->h);
			}
		}
	} else {
		if (blit_op == BLIT_OP_MASK) {
			shift_x_mask((uint8_t *)src, dest, ptr->w, ptr->h);
		} else {
			shift_x((uint8_t *)src, dest, ptr->w, ptr->h);
		}
	}

	return (bmp);
}

//=============================================================================
uint8_t put_bitmaph(uint8_t x, uint8_t y, const Bitmap_t *ptr) __naked
{
	x; y; ptr;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		l, #0
	push	hl			// *1 push L = attr, H = ***
	ld		l, 6 (ix)
	ld		h, 7 (ix)
	push	hl			// *2 push ptr

	ld		l, 4 (ix)
	srl		l
	jr		c, _put_bitmaph_skip1

	ld		h, 5 (ix)
	ld		a, h
	and		a, #3
	jr		nz, _put_bitmaph_skip2

	srl		h
	srl		h

	// (x & 1) == 0 &&& (y & 3) == 0
	push	hl			// *3
	call	_put_bitmap
	ld		sp, ix
	pop		ix
	ret

_put_bitmaph_skip1:
	ld		a, 5 (ix)
	and		a, #3
	ld		h, a
	ld		l, #1
	jr		_put_bitmaph_skip3

_put_bitmaph_skip2:
	ld		h, a
	ld		l, #0

_put_bitmaph_skip3:
	push	hl		// *3
	call	_shift_bitmap
	pop		af		// *3

	pop		af		// *2 ptr
	push	hl		// *2 bmp

	ld		l, 4 (ix)
	srl		l
	ld		h, 5 (ix)
	srl		h
	srl		h
	push	hl		// *3
	call	_put_bitmap
	ld		sp, ix
	pop		ix
	ret
__endasm;

#if 0
	Bitmap_t *bmp;

	if ((x & 1) == 0 && (y & 3) == 0) {
		bmp = (Bitmap_t *)ptr;
	} else {
		bmp = shift_bitmap((x & 1), (y & 3), ptr);
	}

	x >>= 1;
	y >>= 2;

	put_bitmap(x, y, bmp, 0);
#endif
}

//=============================================================================
void put_bitmaph2(uint8_t x, uint8_t y, const Bitmap_t *ptr) __naked
{
	x; y; ptr;
__asm
	ld		a, #BLIT_OP_MASK
	ld		(_blit_op), a

	pop		bc
	pop		de
	pop		hl
	ld		(_put_bitmaph2_ptr+1), hl
	ld		a, l
	add		a, #SIZEOF_BITMAP_T
	ld		l, a
	adc		a, h
	sub		a, l
	ld		h, a
	push	hl
	push	de
	push	bc

_put_bitmaph2_ptr:
	ld		hl, #0
	push	hl
	push	de
	call	_put_bitmaph
	pop		af
	pop		af

	ld		a, #BLIT_OP_OR
	ld		(_blit_op), a

	jp		_put_bitmaph
__endasm;

#if 0
	set_blit_op(BLIT_OP_MASK);
	put_bitmaph(x, y, ptr);
	set_blit_op(BLIT_OP_OR);
	put_bitmaph(x, y, ptr + 1);
#endif
}
