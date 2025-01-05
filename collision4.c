#include "n80dev.h"

//=============================================================================
uint8_t detect_collision4(uint16_t val) __z88dk_fastcall __naked
{
	val;
__asm
	// l = x1 - x2
	// h = w1, w2 = 4
	ld		a, l
	add		a, h
	dec		a		// a = (x1 - x2) + w1 - 1
	inc		h
	inc		h
	inc		h		// h = w1 + 4 - 1 = w1 + 3
	cp		h

	sbc		a, a
	ld		l, a
	ret
__endasm;
}
