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
#ifndef BMP_USE_RENDER_ATTR_PARAM
	ld		e, #80
	ld		a, (_OFFSCR_ADDR+1)
	ld		d, a
	ld		hl, #_mAttrList
	ld		b, #25
	// HL = list_top, DE = attr, B = N lines
#else
	pop		de
	pop		hl
	push	hl
	push	de

	ld		c, l
	ld		b, h		// B = N lines

	ld		h, #80
	call	_get_offscr_addr
	ex		de, hl		// DE = attr top

	ld		a, c
	add		a, a
	add		a, a
	add		a, #<_mAttrList
	ld		l, a
	adc		a, #>_mAttrList
	sub		a, l
	ld		h, a		// HL = list_top
	// HL = list_top, DE = attr, B = N lines
#endif

#ifdef BMP_SMALL_BEEP
	// ------------------------------------------------------ Small Beep On ---
	ld		a, #0x20
_small_beep_inst::
	//out		(0x40), a	// 0xD3 0x40 or LD A, #0x40 0x3E 0x40
	ld		a, #0x40
#endif

	// ------------------------------------------------------------------------
	ld		a, b
	ld		iy, #-2
	add		iy, sp
	di
_render_attr_line_loop:
	ex		af, af
	push	hl
#ifdef USE_LAST_ATTR
	ld		bc, #(ATTRS_PER_LINE * 2 - 2)
#else
	ld		bc, #(ATTRS_PER_LINE * 2)
#endif
_render_attr_loop:
	ldi
	ldi

	jp		po, _render_attr_skip

	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a
	or		a, h
	jr		nz, _render_attr_loop

_render_attr_skip:
	// BC = remain
	ex		de, hl
	add		hl, bc		// HL = DE + BC (last attr)

#ifdef USE_LAST_ATTR
	inc		hl
	ld		a, (hl)
	ld		e, a
	ld		(hl), c		// renew unused attrs

	sub		a, c
	jr		nc, _render_attr_skip2	// if (prev_remain >= remain)

	ld		c, e
	ld		e, l
	ld		d, h
	sbc		hl, bc		// HL = attr + 39 - prev_remain - CY
#else
	xor		a, a
	sub		a, c
	ld		e, l
	ld		d, h		// DE = next line
	jr		z, _render_attr_skip2
#endif
	ld		sp, hl

	ld		hl, #_render_attr_fill_push
	sra		a
	ld		c, a
	dec		b				// B = 0xFF
	add		hl, bc			// HL = _render_attr_fill_push + diff
	ld		bc, #0xe850
	jp		(hl)

	.rept	19
	push	bc
	.endm
_render_attr_fill_push:
	ld		sp, iy

#ifdef USE_LAST_ATTR
	ex		de, hl
_render_attr_skip2:
	ld		de, #80+1
	add		hl, de
#else
_render_attr_skip2:
	ld		hl, #80
	add		hl, de
#endif
	ex		de, hl

	pop		hl
	inc		hl
	inc		hl
	inc		hl
	inc		hl

	ex		af, af
	dec		a
	jr		nz, _render_attr_line_loop
	ei

#ifdef BMP_SMALL_BEEP
	// xor		a, a
	out		(0x40), a
#endif

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif
__endasm;
}

#ifdef BMP_SMALL_BEEP
//=============================================================================
void small_beep_on() __naked
{
__asm
	ld		hl, #_small_beep_inst
	ld		(hl), #0xd3
	ret
__endasm;
}

//=============================================================================
void small_beep_off() __naked
{
__asm
	ld		hl, #_small_beep_inst
	ld		(hl), #0x3e
	ret
__endasm;
}
#endif
