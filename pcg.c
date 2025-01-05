#include "n80dev.h"

//=============================================================================
void init_pcg()
{
	//out_port(3, 8);
	//out_port(15, 54);
__asm
	ld		a, #8
	out		(3), a
	ld		a, #54
	out		(15), a
__endasm;
}

//=============================================================================
void def_pcg(uint8_t ch, const uint8_t *pat) __naked
{
	ch; pat;
__asm
	ld		iy, #0
	add		iy, sp

	ld		a, 2 (iy)	// A = ch
	rlca
	rlca
	rlca
	and		a, #0x03	// for PCG8100
	ld		d, a
	ld		a, 2 (iy)	// A = ch
	add		a, a
	add		a, a
	add		a, a
	ld		e, a

	ld		l, 3 (iy)
	ld		h, 4 (iy)	// HL = pat

	ld		b, #8
_def_pcg_1:
	ld		c, #0
	ld		a, (hl)
	out		(c), a
	inc		c
	out		(c), e
	inc		c
	ld		a, #0x10
	or		a, d
	out		(c), a
	out		(c), d
	inc		hl
	inc		e
	djnz	_def_pcg_1

	ret
__endasm;
}
