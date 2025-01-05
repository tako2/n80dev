#include "n80dev.h"

//=============================================================================
void draw_bytes(uint8_t x, uint8_t y, const uint8_t *bytes, uint8_t num_bytes, uint8_t color) __naked
{
	x; y; bytes; num_bytes; color;
__asm
	push    ix
    ld      ix, #0
    add     ix, sp

    ld      a, 5 (ix)
	cp		#25
	jr		nc, _draw_bytes_ret

    ld      b, 4 (ix)	// x
	ld		c, 8 (ix)	// w
	ld		l, 6 (ix)	// bytes(L)
	ld		h, 7 (ix)	// bytes(H)
	call	_clip_hline

	push	hl	// *1
	push	bc

	ld		a, 9 (ix)	// color
	and		a, a
	jr		z, _draw_bytes_skip

	ld		h, a
	ld		l, c
	ld		c, 5 (ix)
	push	hl
	push	bc
	call	_insert_attr
	pop		bc		// WARNING (may not same)
	pop		hl		// WARNING (may not same)

_draw_bytes_skip:
	ld		h, b
	ld		l, 5 (ix)
    call    _get_offscr_addr	// hl = dest
	pop		bc
	pop		de	// *1
	ex		de, hl
	ld		b, #0
	ldir

_draw_bytes_ret:
    pop     ix
    ret
__endasm;
}
