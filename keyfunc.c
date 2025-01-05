#include "n80dev.h"

static const uint8_t prev_func_key = 0;

uint8_t in_funckey() __naked
{
__asm
	in		a, (9)
	or		a, #0x40	// mask without 'SPACE'
	ld		e, a

	in		a, (8)
	or		a, #0xbf	// mask 'SHIFT'
	and		a, e
	ld		e, a

	ld		hl, #_prev_func_key
	or		a, (hl)
	ld		d, a

	ld		a, #PUSH_FUNC_KEYS
	xor		a, e
	and		a, #PUSH_FUNC_KEYS
	ld		(hl), a

	ld		l, d
	ret
__endasm;
}
