#include "n80dev.h"

//=============================================================================
void draw_msg_list(const MsgList_t *list) __z88dk_fastcall __naked
{
	list;
__asm
_draw_msg_list_loop:
	ld		a, (hl)
	inc		hl
	ld		d, (hl)
	inc		hl
	ld		e, a
	or		a, d
	ret		z

	ld		c, (hl)
	inc		hl
	ld		b, (hl)
	inc		hl
	push	hl

	push	de
	push	bc
	call	_sfnt_puts
	pop		af
	pop		af

	pop		hl
	jr		_draw_msg_list_loop
__endasm;
#if 0
	while (list->msg != NULL) {
		sfnt_puts(list->x, list->y, list->msg);
		list ++;
	}
#endif
}
