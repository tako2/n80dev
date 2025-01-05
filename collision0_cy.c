#include "n80dev.h"

//=============================================================================
uint8_t detect_collision0_cy(uint32_t x1w1x2w2) __z88dk_fastcall __naked
{
	x1w1x2w2;
__asm
	// H=x1, L=w1
	// D=x2, E=w2
	// return with CY (0: no collision, 1: collision)
	dec		l		// L = (w1 - 1)
	ld		a, e
	add		a, l
	ld		e, a	// E = w1 + w2 - 1
	ld		a, h
	sub		a, d
	add		a, l	// A = x1 - x2 + w1 - 1
	cp		e
	ret
__endasm;
}
