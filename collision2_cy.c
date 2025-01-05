#include "n80dev.h"

//=============================================================================
uint8_t detect_collision2_cy(uint16_t val) __z88dk_fastcall __naked
{
	val;
__asm
	// l = x1 - x2
	// h = w1, w2 = 2
	// return with CY (0: no collision, 1: collision)
	ld		a, l
	add		a, h
	dec		a		// a = (x1 - x2) + w1 - 1
	inc		h		// h = w1 + 2 - 1 = w1 + 1
	cp		h
	ret
__endasm;
}
