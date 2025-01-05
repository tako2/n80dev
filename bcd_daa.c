#include "n80dev.h"

//=============================================================================
void daa_func() __naked
{
	// break c reg
__asm
	push	af
	ld		c, a
	and		a, #0x0f
	cp		#0x0a
	jr		c, _daa_func_1
	ld		a, c
	add		a, #0x06
	jr		c, _daa_func_2

	cp		#0xa0
	jr		c, _daa_func_3
	inc		sp
	inc		sp
	add		a, #0x60
	ret			// cf=1

_daa_func_1:
	ld		a, c
	cp		#0xa0
	jr		c, _daa_func_4
	add		#0x60
	inc		sp
	inc		sp
	ret			// cf=1

_daa_func_4:
	pop		af
	ld		a, c
	ret			// cf=original

_daa_func_2:
	inc		sp
	inc		sp
	cp		#0xa0
	ret		c	// cf=1
	add		a, #0x60
	ret			// cf=1

_daa_func_3:
	ld		c, a
	pop		af
	ld		a, c
	ret			// cf=original
__endasm;
}
