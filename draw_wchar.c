#include "n80dev.h"

//=============================================================================
void draw_wchar(uint8_t x, uint8_t y, uint16_t ch, uint8_t color) __naked
{
    x;
    y;
    ch;
    color;
__asm
    push    ix
    ld      ix, #0
    add     ix, sp

    ld      l, 5 (ix)
    ld      h, 4 (ix) 
    call    _get_offscr_addr

    ld      a, 6 (ix)
    ld      (hl), a
	inc		hl
	ld		a, 7 (ix)
    ld      (hl), a

	ld		a, 8 (ix)
	and		a, a
	jr		z, _draw_wchar_ret

	ld		h, a
	ld		l, #0x02
	push	hl
    ld      l, 5 (ix)
    ld      h, 4 (ix) 
	push	hl
	call	_insert_attr

	ld		sp, ix

_draw_wchar_ret:
    pop     ix
    ret
__endasm;
}
