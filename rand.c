#include "n80dev.h"

//=============================================================================
static uint16_t rnd_seed;
void srand(uint16_t seed) __z88dk_fastcall
{
	rnd_seed = seed & 0xfffe;
}

//=============================================================================
// LSFR(6, 14) + R register
// *** random seed must be EVEN number ***
uint8_t rand() __preserves_regs(b, c, d, e, iyh, iyl) __naked
{
__asm
	ld		hl, (_rnd_seed)
	ld		a, h
	xor		a, l
	rlca
	and		a, #0x01
	or		a, l
	ld		l, a
	xor		a, h
	ld		h, l
	ld		l, a

	ld		a, r
	add		a, l

	sla		l
	ld		(_rnd_seed), hl
	ld		l, a
	ret
__endasm;
}

//=============================================================================
uint8_t hit_rand(uint8_t thresh) __z88dk_fastcall __preserves_regs(c, d, e, iyh, iyl) __naked
{
	thresh;
__asm
	ld		b, l
	call	_rand
	ld		a, l

	rrca		/***/

	cp		b
	sbc		a, a
	ld		l, a
	ret
__endasm;
}
