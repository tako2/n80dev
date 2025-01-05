#include "80mkii.h"

//=============================================================================
static const uint8_t pat_table[16] = {
	0b00000000,
	0b00000011,
	0b00001100,
	0b00001111,

	0b00110000,
	0b00110011,
	0b00111100,
	0b00111111,

	0b11000000,
	0b11000011,
	0b11001100,
	0b11001111,

	0b11110000,
	0b11110011,
	0b11111100,
	0b11111111,
};
static void draw_kanji_bits() __naked
{
__asm
	// DE = vram
	// A = bit pattern
	// use BC

	push	af
	rlca
	rlca
	rlca
	rlca
	call	_draw_kanji_bits_sub
	pop		af

_draw_kanji_bits_sub:
	and		a, #0x0f
#if 1
	add		a, #<_pat_table
	ld		c, a
	adc		a, #>_pat_table
	sub		a, c
	ld		b, a
	ld		a, (bc)
#endif
	ld		(de), a
	inc		de
	ret
__endasm;
}

//=============================================================================
void n80_draw_kanji(uint8_t x, uint8_t y, uint16_t addr) __naked
{
	x; y; addr;
__asm
	ld		iy, #0
	add		iy, sp

	ld		h, 2 (iy)	// H = x
	ld		l, 3 (iy)	// L = y
	call	_n80_vram_addr
	ex		de, hl		// DE = vram

	ld		l, 4 (iy)
	ld		h, 5 (iy)	// HL = addr

	call	_n80_enter_vram

	xor		a, a
	out		(0xeb), a
	out		(0xec), a

	ld		b, #16

_n80_draw_kanji_loop:
	push	bc

	ld		a, l
	out		(0xe8), a
	ld		a, h
	out		(0xe9), a
	inc		hl

#if 1
	out		(0xea), a	// read start
	nop
	nop
	in		a, (0xe9)
	call	_draw_kanji_bits
	in		a, (0xe8)
	call	_draw_kanji_bits
	out		(0xeb), a	// read end
#else
	push	hl
	out		(0xea), a	// read start
	nop
	nop
	in		a, (0xe9)
	ld		h, a
	in		a, (0xe8)
	ld		l, a
	out		(0xeb), a

	call	_n80_enter_vram

	ld		a, h
	call	_draw_kanji_bits
	ld		a, l
	call	_draw_kanji_bits

	call	_n80_exit_vram

	pop		hl
#endif

	ld		a, e
	add		a, #80-4
	ld		e, a
	adc		a, d
	sub		a, e
	ld		d, a

	pop	bc
	djnz	_n80_draw_kanji_loop

	call	_n80_exit_vram

	ret
__endasm;
}

//=============================================================================
void n80_draw_kanji_str(uint8_t x, uint8_t y, const uint16_t *str)
{
	while (*str != 0) {
		n80_draw_kanji(x, y, *str);
		str ++;
		x += 4;
	}
}
