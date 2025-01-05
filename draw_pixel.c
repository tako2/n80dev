#include "n80dev.h"

//=============================================================================
static const uint8_t pixel_bit[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

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
	xor		a, (hl)
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
#ifdef USE_CLEAR_PIXEL
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
#endif
