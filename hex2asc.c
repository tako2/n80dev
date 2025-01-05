#include "n80dev.h"

//=============================================================================
void hex2asc(uint8_t *dest, uint8_t val) __naked
{
	dest; val;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)
	ld		a, 4 (iy)

	push	af
	rlca
	rlca
	rlca
	rlca
	and		a, #0x0f
	add		a, #0x90
	daa
	adc		a, #0x40
	daa
	ld		(hl), a
	inc		hl

	pop		af
	and		a, #0x0f
	add		a, #0x90
	daa
	adc		a, #0x40
	daa
	ld		(hl), a

	ret
__endasm;
}
