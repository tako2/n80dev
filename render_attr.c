#include "n80dev.h"

#ifndef USE_LAST_ATTR
#if (ATTRS_PER_LINE == 20)
#define USE_LAST_ATTR
#endif
#endif

//=============================================================================
void render_attr(
#ifdef BMP_USE_RENDER_ATTR_PARAM
						uint8_t y, uint8_t h
#endif
) __naked
{
#ifdef BMP_USE_RENDER_ATTR_PARAM
	y; h;
#endif

__asm
#ifdef CLEAR_ATTR_FIRST
	call	_clear_vram_attr
#endif

#ifndef BMP_USE_RENDER_ATTR_PARAM
#ifdef USE_LAST_ATTR
	ld		e, #80+(ATTRS_PER_LINE * 2)-1 //39
#else
	ld		e, #80
#endif
	ld		a, (_OFFSCR_ADDR+1)
	ld		d, a
	ld		hl, #_mAttrList
	ld		b, #25
	// HL = list_top, DE = attr, B = N lines
#else
	ld		hl, #3
	add		hl, sp

	ld		b, (hl)		// B = h
	dec		hl
	ld		a, (hl)		// L = y
	push	af
	ld		l, a
#ifdef USE_LAST_ATTR
	ld		h, #80+(ATTRS_PER_LINE * 2)-1 //39
#else
	ld		h, #80
#endif
	call	_get_offscr_addr
	ex		de, hl
	pop		af
	add		a, a
	add		a, a
	add		a, #<_mAttrList
	ld		l, a
	adc		a, #>_mAttrList
	sub		a, l
	ld		h, a
	// HL = list_top, DE = attr, B = N lines
#endif

#ifndef CLEAR_ATTR_FIRST
	ld		a, #0x20
_small_beep_inst::
	//out		(0x40), a	// 0xD3 0x40 or LD A, #0x40 0x3E 0x40
	ld		a, #0x40
#endif

_render_attr_loop:
	push	bc
	push	hl		// *2 list_top

#ifndef CLEAR_ATTR_FIRST
#ifdef USE_LAST_ATTR
	push	de		// *1 attr

	ld		bc, #-((ATTRS_PER_LINE * 2)-1)	//39
	ex		de, hl
	add		hl, bc
	ex		de, hl
	ld		bc, #(ATTRS_PER_LINE * 2)-2		//38
#else
	ld		bc, #(ATTRS_PER_LINE * 2)
#endif
	ldi
	ldi

	ld		a, (hl)	// 7
	inc		hl		// 6
	ld		h, (hl)	// 7
	ld		l, a	// 4
	or		a, h	// 4
	jr		z, _render_attr_fill_skip0
#else
#ifdef USE_LAST_ATTR
	push	de		// *1 attr
#endif
	ld		bc, #(NUM_LIMITED_ATTR*2)
#endif

_render_attr_line_loop:
	ldi				// 16
	ldi				// 16
	jp		po, _render_attr_fill_skip0	// 10
	ld		a, (hl)	// 7
	inc		hl		// 6
	ld		h, (hl)	// 7
	ld		l, a	// 4
	or		a, h	// 4
	jr		nz, _render_attr_line_loop		// 7/12
	// total = 70 + 7/12

_render_attr_fill_skip0:
#ifndef CLEAR_ATTR_FIRST
#ifdef USE_LAST_ATTR
	pop		hl		// *1 attr+39
	ld		a, (hl)
	ld		(hl), c
	sub		a, c
	ex		de, hl	// DE = prev attr last

	jr		nc, _render_attr_fill_skip
	neg
	ld		c, a

	ld		iy, #0	// 14
	add		iy, sp	// 15
	di

	add		hl, bc
	ld		sp, hl

	ld		hl, #_render_attr_fill_push
	srl		c
	sbc		hl, bc
	ld		bc, #0xe850
	jp		(hl)
#else
	ld		a, c
	or		a, a
	jr		z, _render_attr_fill_skip

	ex		de, hl
	add		hl, bc

	ld		iy, #0
	add		iy, sp
	di

	ld		sp ,hl
	ex		de, hl
	ld		hl, #_render_attr_fill_push
	srl		c
	sbc		hl, bc
	ld		bc, #0xe850
	jp		(hl)
#endif

	.rept	19
	push	bc
	.endm
_render_attr_fill_push:

	ld		sp, iy
	ei
_render_attr_fill_skip:
#endif

#ifndef CLEAR_ATTR_FIRST
#ifdef USE_LAST_ATTR
	ld		hl, #BYTES_PER_LINE
#else
	ld		hl, #80
#endif
	add		hl, de
#else
#ifdef USE_LAST_ATTR
	pop		hl		// *1 attr
#endif
	ld		de, #BYTES_PER_LINE
	add		hl, de
#endif
	pop		de		// *2 list_top
	ex		de, hl
	ld		bc, #4
	add		hl, bc	// list_top += 4
	pop		bc
	djnz	_render_attr_loop

#ifndef CLEAR_ATTR_FIRST
	xor		a, a
	out		(0x40), a
#endif

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif
__endasm;
}

#if 0
{
	AttrList_t *ptr;
	uint8_t line;
	uint8_t *attr;
	uint8_t chg;

	for (line = 0; line < 25; line ++) {
		attr = get_offscr_addr(0x5000+line);
		ptr = &mAttrList[line];

		chg = 0;
		while (ptr != NULL && chg < 20) {
			*attr ++ = ptr->x;
			*attr ++ = ptr->attr;

			ptr = ptr->next;
			chg ++;
		}
	}
}
#endif
