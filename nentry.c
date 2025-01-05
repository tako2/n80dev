#include "n80dev.h"

NameEntry_t nentry_list[NUM_NENTRY];

//=============================================================================
void init_nentry(const NameEntry_t *template) __z88dk_fastcall __naked
{
	template;
__asm
	ld		de, #_nentry_list
	ld		a, #NUM_NENTRY
	ld		b, #0
_init_nentry_loop:
	push	hl
	ld		c, #8	// sizeof(NameEntry_t)
	ldir
	pop		hl
	dec		a
	jr		nz, _init_nentry_loop
	ret
__endasm;
}

//=============================================================================
int8_t insert_nentry(NameEntry_t *new) __z88dk_fastcall __naked
{
	new;
__asm
	inc		hl
	inc		hl
	ld		de, #_nentry_list+2
	ex		de, hl
	// HL = &nentry_list[0].score[2], DE = &new.score[2]

	ld		c, #NUM_NENTRY

_insert_nentry_loop:
	push	bc
	push	de
	push	hl
	call	_bcd24_compare
	ld		a, l
	pop		hl
	pop		de
	pop		bc
	or		a, a
	jp		m, _insert_nentry_hit

	ld		a, #8	// sizeof(NameEntry_t)
	add		a, l
	ld		l, a
	adc		a, h
	sub		a, l
	ld		h, a

	dec		c
	jr		nz, _insert_nentry_loop
	ld		l, #-1
	ret

_insert_nentry_hit:
	ld		a, #NUM_NENTRY
	sub		a, c
	ex		af, af	// to return index

	dec		hl
	dec		hl
	dec		de
	dec		de
	// HL = nentry_list[NUM_NENTRY-C], DE = new

	dec		c
	jr		z, _insert_nentry_skip

	ld		a, c
	add		a, a
	add		a, a
	add		a, a	// A = sizeof(NameEntry_t) * numn_items
	ld		c, a
	ld		b, #0

	add		hl, bc
	dec		hl
	push	de
	ex		de, hl
	ld		hl, #8
	add		hl, de
	// HL = last, DE = last - 1
	ex		de, hl
	lddr
	inc		hl
	pop		de

_insert_nentry_skip:
	ex		de, hl
	ld		bc, #8	// sizeof(NameEntry_t)
	ldir

	ex		af, af
	ld		l, a
	ret
__endasm;
}

//=============================================================================
void copy_nentry_name(uint8_t rank, uint8_t *name) __naked
{
	rank; name;
__asm
	ld		iy, #0
	add		iy, sp

	ld		a, 2 (iy)
	add		a, a
	add		a, a
	add		a, a	// A = rank x 8 (sizeof(NameEntry_t))

	add		a, #3	// skip score[3]
	add		a, #<_nentry_list
	ld		l, a
	adc		a, #>_nentry_list
	sub		a, l
	ld		h, a

	ld		e, 3 (iy)
	ld		d, 4 (iy)
	ld		bc, #4	// sizeof(NameEntry_t.name[4])
	ex		de, hl
	ldir

	ret
__endasm;
}

//=============================================================================
NameEntry_t *get_nentry(uint8_t rank) __z88dk_fastcall __preserves_regs(b, c, d, e, iyh, iyl) __naked
{
	rank;
__asm
	ld		a, l
	add		a, a
	add		a, a
	add		a, a	// A = L x 8 (sizeof(NameEntry_t))

	add		a, #<_nentry_list
	ld		l, a
	adc		a, #>_nentry_list
	sub		a, l
	ld		h, a

	ret
__endasm;
}
