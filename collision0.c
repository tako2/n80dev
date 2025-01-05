#include "n80dev.h"

//=============================================================================
uint8_t detect_collision0(uint32_t x1w1x2w2) __z88dk_fastcall __naked
{
	x1w1x2w2;
__asm
	// H=x1, L=w1
	// D=x2, E=w2
#if 0
	ld		a, h
	sub		a, d	// A = x1 - x2
	dec		l		// L = (w1 - 1)
	add		a, l
	ld		h, a	// H = x1 - x2 + w1 - 1
	ld		a, l
	add		a, e	// A = w1 + w2 - 1
	ld		l, a
	ld		a, h
	cp		l
#else
	dec		l		// L = (w1 - 1)
	ld		a, e
	add		a, l
	ld		e, a	// E = w1 + w2 - 1
	ld		a, h
	sub		a, d
	add		a, l	// A = x1 - x2 + w1 - 1
	cp		e
#endif
	sbc		a, a
	ld		l, a
	ret
__endasm;
}
