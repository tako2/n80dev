#include "n80dev.h"

//=============================================================================
uint8_t detect_collision(uint8_t x1, uint8_t x2, uint8_t w1, uint8_t w2) __preserves_regs(d, e, iyh, iyl) __naked
{
	x1; x2; w1; w2;
__asm
	ld		hl, #5
	add		hl, sp

	ld		a, (hl)	// A = w2
	dec		hl
	ld		b, (hl)
	dec		hl
	dec		b		// B = (w1 - 1)
	add		a, b
	ld		c, a	// C = (w1 - 1) + w2
	ld		a, b
	sub		a, (hl)	// A = - x2 + (w1 - 1)
	dec		hl
	add		a, (hl)	// A = x1 - x2 + (w1 - 1)
	cp		c

	sbc		a, a
	ld		l, a
	ret
	// total = 105 clk
__endasm;
}
