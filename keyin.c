#include "n80dev.h"

//#define PUSH_KEYS (KEY_X | KEY_Z | KEY_SPACE | KEY_RET)

static const uint8_t prev_key = 0;

uint8_t in_tenkey() __preserves_regs(b, c, iyh, iyl) __naked
{
__asm
	in		a, (0)
	ld		h, a
	or		a, #0xab	// mask '2', '4', '6'
	ld		l, a
	ld		a, h
	or		a, #0xd5	// mask '1', '3', '5'
	rlca
	rlca
	rlca
	and		a, l
	ld		l, a

	in		a, (1)
	or		a, #0x7e	// mask 'RET', '8'
	and		a, l
	ld		l, a

	in		a, (5)
	rla
	or		a, #0xf5	// mask 'X', 'Z'
	and		a, l
	ld		l, a

	in		a, (9)
	rra
	or		a, #0xdf	// mask 'SPACE'
	and		a, l

#if 1
	ld		e, a
	ld		hl, #_prev_key
	or		a, (hl)
	ld		d, a

	ld		a, #PUSH_KEYS
	xor		a, e
	and		a, #PUSH_KEYS
	ld		(hl), a
	ld		l, d
#else
	ld		h, a
	ld		a, (_prev_key)
	or		a, h
	ld		l, a
	ld		a, h
	xor		a, #PUSH_KEYS
	and		a, #PUSH_KEYS
	ld		(_prev_key), a
#endif

	ret
__endasm;
}
