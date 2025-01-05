#include "n80dev.h"

//=============================================================================
void draw_char(uint8_t x, uint8_t y, uint8_t ch, uint8_t color) __naked
{
    x; y; ch; color;
__asm
	ld		hl, #5
	add		hl, sp

	ld		c, (hl)		// C = attr
	dec		hl
	ld		b, (hl)		// B = ch
	dec		hl
	ld		a, (hl)
	dec		hl
	ld		h, (hl)
	ld		l, a		// H = x, L = y
	push	hl
	call	_get_offscr_addr
	ld		(hl), b
	pop		hl

	ld		a, c
	or		a, a
	jp		nz, _insert_char_attr_reg
	ret

#if 0
    push    ix
    ld      ix, #0
    add     ix, sp

    ld      l, 5 (ix)	// L = y
    ld      h, 4 (ix)	// H = x
    call    _get_offscr_addr	// HL = dest

    ld      a, 6 (ix)	// A = ch
    ld      (hl), a

	ld		a, 7 (ix)	// A = color
	and		a, a
	jr		z, _draw_char_ret

#if 0
	ld		h, a
	ld		l, #0x01
	push	hl
    ld      l, 5 (ix)
    ld      h, 4 (ix) 
	push	hl
	call	_insert_attr
#else
	push	af
	inc		sp
    ld      l, 5 (ix)
    ld      h, 4 (ix) 
	push	hl
	call	_insert_char_attr
#endif

	ld		sp, ix

_draw_char_ret:
    pop     ix
    ret
#endif
__endasm;
}

