#include "n80dev.h"

//=============================================================================
#ifdef FNT_USE_SFONT_DEFAULT
#include "font3x4_pat.h"
#else
extern const uint8_t font3x4_array[];
#endif
// const uint8_t font3x4_array[] = {...};

//=============================================================================
void sfnt_putch() __naked
{
__asm
	// input
	//		A: ch no (0x00~0x40)
	//		HL: dest vram addr
	// output
	//		A: (broken)
	//		HL: +2 ascended
	push	de
	ex		de, hl

	add		a, a
	add		a, #<_font3x4_array
	ld		l, a
	adc		a, #>_font3x4_array
	sub		a, l
	ld		h, a

	ldi
	ldi
	inc		bc
	inc		bc

	ex		de, hl
	pop		de
	ret
__endasm;
}

//=============================================================================
void sfnt_puts(uint8_t x, uint8_t y, const uint8_t *str) __naked
{
	x; y; str;
__asm
	pop		hl
	pop		de
	pop		bc
	push	bc
	push	de
	push	hl

	ld		l, d
	ld		h, e
	call	_get_offscr_addr	// HL = VRAM addr
	ld		d, b
	ld		e, c		// DE = str

_sfnt_puts_2:
	ld		a, (de)
	inc		de
	and		a, a
#ifdef COUNT_VRTC_IN_LIB
	jp		z, _count_vrtc
#else
	ret		z
#endif

	sub		a, #0x20
	cp		#0x40
	jr		nc, _sfnt_puts_1

_sfnt_putch_stub::
	call	_sfnt_putch
	jr		_sfnt_puts_2

_sfnt_puts_1:
	inc		hl
	inc		hl
	jr		_sfnt_puts_2
__endasm;
}

//=============================================================================
void sfnt_put_bcd(uint8_t x, uint8_t y, uint8_t *bcd, uint8_t keta) __naked
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
#ifdef COUNT_VRTC_IN_LIB
	jp		nz, _count_vrtc
#else
	ret		nz
#endif

	dec		hl
	dec		hl
	ld		a, #0x10
	call	_sfnt_putch
#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif
__endasm;
}

//=============================================================================
void sfnt_make_bmp(const uint8_t *str, uint8_t *dest) __naked
{
	str; dest;
__asm
	ld		hl, #2
	add		hl, sp
	ld		e, (hl)
	inc		hl
	ld		d, (hl)
	inc		hl
	ld		c, (hl)
	inc		hl
	ld		b, (hl)

_sfnt_make_bmp_loop:
	ld		a, (de)
	add		a, a
	ret		z

	sub		a, #0x40

	add		a, #<_font3x4_array
	ld		l, a
	adc		a, #>_font3x4_array
	sub		a, l
	ld		h, a

	ld		a, (hl)
	inc		hl
	ld		(bc), a
	inc		bc
	ld		a, (hl)
	ld		(bc), a
	inc		bc

	inc		de
	jr		_sfnt_make_bmp_loop

__endasm;
}
