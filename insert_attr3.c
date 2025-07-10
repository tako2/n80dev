#include "n80dev.h"

//#define CHECK_ATTR

//#define SEMI_OFF
//#define GREEN_MODE

//#define CLEAR_ATTR_FIRST

#define NUM_LIMITED_ATTR 5

//#define CHECK_FREE_ATTR

AttrList_t mAttrList[25];

//#define LARGE_ATTR_WORK
#define NUM_ATTR_LIST_WORK 256
AttrList_t mAttrListWork[NUM_ATTR_LIST_WORK];
AttrList_t *mFreeAttr;

#ifdef CHECK_FREE_ATTR
#ifndef LARGE_ATTR_WORK
uint8_t num_free_attr;
#else
uint16_t num_free_attr;
#endif
#endif

#ifdef CHECK_ATTR
//=============================================================================
void check_attr_line(AttrList_t *node)
{
	node;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)

	ld		c, #0

	_check_attr_line_loop:
	ld		a, l
	or		a, h
	jr		z, _check_attr_line_break

	ld		a, (hl)
	and		a, a
	jr		z, _check_attr_line_skip
	cp		c
	jr		z, _check_attr_line_halt1
	jr		c, _check_attr_line_halt1
	cp		#80
	jr		z, _check_attr_line_halt2

	_check_attr_line_skip:
	ld		c, a

	push	hl
	inc		hl
	inc		hl
	ld		e, (hl)
	inc		hl
	ld		d, (hl)
	pop		hl

	ex		de, hl

	ld		a, l
	cp		e
	jr		nz, _check_attr_line_loop
	ld		a, h
	cp		d
	jr		nz, _check_attr_line_loop

	di
	halt

	_check_attr_line_halt1:
	di
	halt

	_check_attr_line_halt2:
	di
	halt

	_check_attr_line_break:
	ret
__endasm;
}

//=============================================================================
void check_attr()
{
	uint8_t line;
	AttrList_t *node;
	//int8_t prev_x;

	for (line = 0; line < 25; line ++) {
		node = &mAttrList[line];

		check_attr_line(node);
	}
}
#endif

//=============================================================================
void clear_attr(uint8_t attr) __z88dk_fastcall __naked
{
	attr;
	// 650+(20 or 26)+36 = 706 or 712
__asm
	ld		iy, #0		// 14
	add		iy, sp		// 15

#ifdef SEMI_OFF
	ld		a, l
	and		a, #0xef
	ld		l, a
#endif
#ifdef GREEN_MODE
	ld		l, #0x9f
#endif

	di					// 4
	ld		sp, #_mAttrList + (25 * 4)	// 10
	ld		h, l		// 4
	ld		l, #0		// 7
	ld		bc, #0		// 10
	.rept	25
	push	bc			// 11
	push	hl			// 11
	.endm				// 22x25 = 550
	ld		sp, iy		// 10
	ei					// 4

#ifdef CHECK_FREE_ATTR
#ifndef LARGE_ATTR_WORK
	ld		a, #(NUM_ATTR_LIST_WORK - 1)	// 7
	ld		(_num_free_attr), a				// 13
#else
	ld		hl, #NUM_ATTR_LIST_WORK			// 10
	ld		(_num_free_attr), hl			// 16
#endif
#endif

	ld		hl, #_mAttrListWork+(NUM_ATTR_LIST_WORK*4)		// 10
	ld		(_mFreeAttr), hl		// 16

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret					// 10
#endif
__endasm;
}

//=============================================================================
#ifdef BMP_USE_CLEAR_ATTR_MAPPED2
void clear_attr_mapped2(const uint8_t *table) __z88dk_fastcall __naked
{
	table;
__asm
	// 81 clk
	ld		iy, #0
	add		iy, sp

	ld		bc, #24
	add		hl, bc		// HL = table + 24

	di
	ld		sp, #_mAttrList + (25 * 4)
	ld		e, #0
	ld		bc, #0

	// 875 clk
	.rept	25
	push	bc
	ld		d, (hl)
	dec		hl
#ifdef SEMI_OFF
	ld		a, d
	and		a, #0xef
	ld		d, a
#endif
#ifdef GREEN_MODE
	ld		d, #0x9f
#endif
	push	de
	.endm

	// 67 clk
	ld		sp, iy
	ei

#ifdef CHECK_FREE_ATTR
	ld		a, #NUM_ATTR_LIST_WORK-1
	ld		(_num_free_attr), a
#endif

	ld		hl, #_mAttrListWork+(NUM_ATTR_LIST_WORK*4)
	ld		(_mFreeAttr), hl

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc		// about 46 clk
#else
	ret
#endif
	// total 1069 clk
__endasm;
}
#endif

//=============================================================================
#define alloc_param(x, attr, next) ((uint32_t)(x) | (uint32_t)((attr) << 8) | (uint32_t)(next) << 16)
AttrList_t *alloc_attr(uint32_t attr_x) __z88dk_fastcall __naked
{
#if 0
	AttrList_t *new;

	if (num_free_attr != 0) {
		new = mFreeAttr;
		new->next = NULL;
		mFreeAttr ++;
		num_free_attr --;
		return (new);
	}
	else {
		// assert();
		while (1) ;
		return (NULL);
	}
#else
	attr_x;
__asm
	// L = x
	// H = attr
	// DE = next
	ld		c, l	// C = x		    4
	ld		b, h	// B = attr		    4

#ifdef CHECK_FREE_ATTR
#ifndef LARGE_ATTR_WORK
	ld		hl, #_num_free_attr
	dec		(hl)
	jr		z, _alloc_attr_skip
	// 36
#else
	ld		hl, #_num_free_attr
	ld		a, (hl)
	inc		hl
	or		a, (hl)
	jr		z, _alloc_attr_skip

	dec		hl
	dec		(hl)
	inc		hl
	sbc		a, a
	add		a, (hl)
	ld		(hl), a
#endif
#endif

	ld		hl, (_mFreeAttr)
	dec		hl
	ld		(hl), d
	dec		hl
	ld		(hl), e
	dec		hl
	ld		(hl), b
	dec		hl
	ld		(hl), c
	ld		(_mFreeAttr), hl
	ret
	// 94

	// total = 36+94 = 130

#ifdef CHECK_FREE_ATTR
_alloc_attr_skip:
	di
	halt
#endif
__endasm;
#endif
}

//=============================================================================
static void alloc_attr_macro() __naked
{
__asm
	// input:
	// C = x
	// B = attr
	// DE = next
	// output:
	// HL, SP = alloc attr
	.macro	_alloc_attr_m
	ld		sp, (_mFreeAttr)
	push	de
	push	bc
	ld		(_mFreeAttr), sp
	ld		hl, (_mFreeAttr)
	// total = 78
	.endm
__endasm;
}

#if 0
//=============================================================================
void replace_attr(AttrList_t *start, uint8_t x, uint8_t rx, uint8_t attr)
{
	AttrList_t *node, *prev, *new, *new2;
	int8_t N;

	prev = start;
	node = prev->next;
	N = 0;
	while (node != NULL) {
		if (rx == node->x) {
			if (attr == node->attr) {
				prev = node;
				node = node->next;
				N ++;
			}
			if (N == 0) {
				start->next = alloc_attr(alloc_param(x, attr, node));
			} else {
				prev->x = x;
				prev->attr = attr;
				start->next = prev;
			}
			return;
		}
		else if (rx < node->x) {
			break;
		}
		prev = node;
		node = node->next;
		N ++;
	}

	if (attr != prev->attr && rx < 80) {
		AttrList_t *next;
		switch (N) {
		case 0:
			// add 2 node
			next = alloc_attr(alloc_param(rx, prev->attr, node));
			start->next = alloc_attr(alloc_param(x, attr, next));
			break;
		case 1:
			// add 1 node
			prev->x = rx;
			start->next = alloc_attr(alloc_param(x, attr, prev));
			break;
		default:
			next = start->next;
			next->x = x;
			next->attr = attr;
			next->next = prev;
			prev->x = rx;
			break;
		}
	} else {
		prev->x = x;
		prev->attr = attr;
		start->next = prev;
	}
}
#else
//=============================================================================
void replace_attr(AttrList_t *start, uint8_t x, uint8_t rx, uint8_t attr) __naked
{
	start; x; rx; attr;
__asm
	ld		iy, #-2
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)

	ld		c, 5 (iy)	// C = rx
	ld		b, #0		// B = N
	// 108

_replace_attr_loop:
	ld		e, l
	ld		d, h
	inc		hl
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a

	or		a, h
	// 48
	jr		z, _replace_attr_while_break

	ld		a, c
	cp		(hl)		// compare rx with node->x
	jr		z, _replace_attr_same_x
	jr		c, _replace_attr_large

	inc		b
	jr		_replace_attr_loop
	// 96

_replace_attr_same_x:
	ld		a, 6 (iy)	// A = attr
	inc		hl
	cp		(hl)		// compare attr with node->attr
	dec		hl
	jr		nz, _replace_attr_same_x_skip1
	// 45/50

	ld		e, l
	ld		d, h
	inc		hl
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a		// prev = node, node = node->next

	inc		b
	// 48

_replace_attr_same_x_skip1:
	ld		a, b
	and		a, a
	jr		nz, _replace_attr_same_x_skip2

	ex		de, hl		// DE = node
	ld		l, 4 (iy)	// L = x
	ld		h, 6 (iy)	// H = attr
	call	_alloc_attr	// 151+17 = 168

	ex		de, hl
	ld		l, 2 (iy)
	ld		h, 3 (iy)
	inc		hl
	inc		hl
	ld		(hl), e
	inc		hl
	ld		(hl), d		// start->next = _alloc_attr

	jr		_replace_attr_ret
	// 323
	// from _replace_attr_same_x to here = 373 or 416

_replace_attr_same_x_skip2:
	ld		l, 4 (iy)	// L = x
	ld		h, 6 (iy)	// H = attr
	ex		de, hl
	ld		c, l
	ld		b, h
	ld		(hl), e		// prev->x = x
	inc		hl
	ld		(hl), d		// prev->attr = attr

	ld		l, 2 (iy)
	ld		h, 3 (iy)
	inc		hl
	inc		hl
	ld		(hl), c
	inc		hl
	ld		(hl), b		// start->next = prev

	jr		_replace_attr_ret
	// 164
	// from _replace_attr_same_x to here = 234 or 277
	// from _replace_attr_while_break to here = 187 or 227

_replace_attr_while_break:
	ld		a, c
	cp		#80
	jr		nc, _replace_attr_same_x_skip2

_replace_attr_large:
	inc		de
	ld		a, (de)
	dec		de
	cp		6 (iy)		// compare prev->attr with attr
	jr		z, _replace_attr_same_x_skip2

	ld		a, b
	sub		a, #1

	jr		z, _replace_attr_large_ins_1
	jr		c, _replace_attr_large_ins_2

	// default
	ld		l, 2 (iy)
	ld		h, 3 (iy)
	inc		hl
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a		// HL = start->next

	ld		a, 4 (iy)	// x
	ld		(hl), a		// start->next->x = x
	inc		hl
	ld		a, 6 (iy)	// attr
	ld		(hl), a		// start->next->attr = attr
	inc		hl
	ld		(hl), e
	inc		hl
	ld		(hl), d		// start->next->next = prev

	ex		de, hl
	ld		(hl), c		// prev->x = rx

	jr		_replace_attr_ret
	// from _replace_attr_while_break to here = 281

_replace_attr_large_ins_1:
	ld		a, c
	ld		(de), a		// prev->x = rx

	// DE = prev
	ld		l, 4 (iy)	// x
	ld		h, 6 (iy)	// attr
	call	_alloc_attr	// 151+17 = 168

	ex		de, hl
	ld		l, 2 (iy)
	ld		h, 3 (iy)	// HL = start, DE = new
	inc		hl
	inc		hl
	ld		(hl), e
	inc		hl
	ld		(hl), d		// start->next = new

	jr		_replace_attr_ret
	// 164+151 = 315
	// from _replace_attr_while_break to here = 411

_replace_attr_large_ins_2:
	// prev == start
	push	de			// *1 prev (start)

	ex		de, hl		// DE = node
	inc		hl
	ld		h, (hl)		// H = start->attr
	ld		l, c		// L = rx
	call	_alloc_attr	// 151+17 = 168

	ex		de, hl		// DE = next
	ld		l, 4 (iy)	// L = x
	ld		h, 6 (iy)	// H = attr
	call	_alloc_attr	// 151+17 = 168

	pop		de			// *1 DE = start
	ex		de, hl		// HL = start, DE = new
	inc		hl
	inc		hl
	ld		(hl), e
	inc		hl
	ld		(hl), d		// start->next = new

_replace_attr_ret:
	ld		sp, ix

	pop		ix
#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif
	// 178+302 = 480
	// from _replace_attr_while_break to here = 583
__endasm;
}
#endif

#if 0
//=============================================================================
void replace_attr_just(AttrList_t *start, AttrList_t *prev,
					   uint8_t rx, uint8_t attr, int8_t del)
{
	AttrList_t *node, *new;

	node = prev->next;
	while (node != NULL) {
		if (rx == node->x) {
			if (attr != node->attr) {
				start->next = node;
			} else {
				start->next = node->next;
			}
			start->attr = attr;
			return;
		}
		else if (rx < node->x) {
			break;
		}
		prev = node;
		node = node->next;
		del ++;
	}

	if (attr != prev->attr && rx < 80) {
		if (del == 0) {
			start->next = alloc_attr(alloc_param(rx, prev->attr, node));
			start->attr = attr;
		} else {
			prev->x = rx;
			start->attr = attr;
			start->next = prev;
		}
	} else {
		start->attr = attr;
		start->next = node;
	}
}
#else
void replace_attr_just(AttrList_t *start, AttrList_t *prev,
					   uint8_t rx, uint8_t attr, int8_t del) __naked
{
	start; prev; rx; attr; del;
__asm
	ld		iy, #-2
	add		iy, sp

	ld		l, 4 (iy)
	ld		h, 5 (iy)	// HL = prev

	ld		c, 6 (iy)	// C = rx
	ld		b, 8 (iy)	// B = del
	// 120

_replace_attr_just_loop:
	ld		e, l
	ld		d, h
	inc		hl
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a		// HL = node = prev->next, DE = prev

	or		a, h
	jr		z, _replace_attr_just_while_break

	ld		a, c
	cp		(hl)		// compare rx with node->x
	jr		z, _replace_attr_just_same_x
	jr		c, _replace_attr_just_large

	inc		b
	jr		_replace_attr_just_loop
	// 96

_replace_attr_just_same_x:
	ld		a, 7 (iy)	// A = attr
	inc		hl
	cp		(hl)		// compare with node->attr
	dec		hl			// HL = node
	jr		nz, _replace_attr_just_same_x_skip
	inc		hl
	inc		hl
	ld		b, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, b		// HL = node->next
_replace_attr_just_same_x_skip:
	ld		e, 2 (iy)
	ld		d, 3 (iy)
	ex		de, hl		// HL = start, DE = node or node->next
	inc		hl
	ld		(hl), a		// start->attr = A
	inc		hl
	ld		(hl), e
	inc		hl
	ld		(hl), d		// start->next = DE

	ld		sp, ix

#ifdef COUNT_VRTC_IN_LIB
	call	_count_vrtc
#endif

	pop		ix
	ret
	// from _replace_attr_just_same_x to here = 186 or 155

_replace_attr_just_while_break:
	ld		a, c
	cp		#80
	jr		nc, _replace_attr_just_large_skip3	// HL = NULL, DE = prev

_replace_attr_just_large:
	ld		a, 7 (iy)	// A = attr
	ex		de, hl
	inc		hl
	cp		(hl)		// compare with prev->attr, HL = &prev->attr, DE = node
	jr		z, _replace_attr_just_large_skip

	ld		a, b
	and		a, a		// del is 0 ?
	jr		nz, _replace_attr_just_large_skip2

	// DE = node
	ld		h, (hl)
	ld		l, c		// HL = (rx, prev->attr)
	call	_alloc_attr	// 151+17 = 168

	jr		_replace_attr_just_large_skip3

_replace_attr_just_large_skip2:
	dec		hl
	ld		(hl), c		// prev->x = rx

_replace_attr_just_large_skip3:
	ex		de, hl		// DE = prev or new
	ld		a, 7 (iy)	// A = attr

_replace_attr_just_large_skip:
	ld		l, 2 (iy)
	ld		h, 3 (iy)	// HL = start
	inc		hl
	ld		(hl), a		// start->attr = attr
	inc		hl
	ld		(hl), e
	inc		hl
	ld		(hl), d		// start->next = node or prev or new

	ld		sp, ix

#ifdef COUNT_VRTC_IN_LIB
	call	_count_vrtc
#endif

	pop		ix
	ret
__endasm;
}
#endif

//=============================================================================
void insert_attr(uint8_t line, uint8_t x, uint8_t w, uint8_t attr) __naked
{
	line; x; w; attr;
#if 0
	AttrList_t *node, *prev, *new;
	uint8_t rx;

	node = &mAttrList[line];

	rx = x + w;

	if (x == 0) {
		// replace first node
		replace_attr_just(node, node, rx, attr, 0);
		assert(node->next != node);
		return;
	}

	prev = node;
	node = node->next;
	while (node != NULL) {
		if (x == node->x) {
			if (attr != prev->attr) {
				replace_attr_just(node, node, rx, attr, 0);
			} else {
				//x = prev->x;
				replace_attr_just(prev, node, rx, attr, 1);
			}
			assert(node->next != node);
			return;
		}
		else if (x < node->x) {
			if (attr != prev->attr) {
				replace_attr(prev, x, rx, attr);
			} else {
				//x = prev->x;
				replace_attr_just(prev, prev, rx, attr, 0);
			}
			assert(node->next != node);
			return;
		}
		prev = node;
		node = node->next;
	}

	if (prev->attr != attr) {
		new = alloc_attr(alloc_param(x, attr, NULL));
		prev->next = new;
		if (rx < 80) {
			new->next = alloc_attr(alloc_param(rx, prev->attr, NULL));
		}
		assert(node->next != node);
	}
#else
__asm
#ifdef GREEN_MODE
	ret
#endif

	push	ix
	ld		ix, #0
	add		ix, sp

	ld		a, 4 (ix)		// line
	add		a, a
	add		a, a
	add		a, #<_mAttrList
	ld		l, a
	adc		a, #>_mAttrList
	sub		a, l
	ld		h, a			// HL = node = &mAttrList[line]

	ld		a, 5 (ix)		// x
	ld		e, a
	add		a, 6 (ix)		// A = rx = x + w
	ld		c, a
	ld		b, 7 (ix)		// BC = (attr, rx)
#ifdef SEMI_OFF
	ld		a, b
	and		a, #0xef
	ld		b, a
#endif

	ld		a, e
	and		a, a
	jp		z, _insert_attr_x0
	// 183

_insert_attr_loop:
	ld		e, l
	ld		d, h			// DE = prev = node
	inc		hl
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a			// HL = node->next

	or		a, h
	jr		z, _insert_attr_while_break

	ld		a, 5 (ix)		// x
	cp		(hl)
	jr		z, _insert_attr_same_x
	jr		nc, _insert_attr_loop
	// loop = 100

	// ------------------------------------------------------------------------
	// case (x < node->x)
	inc		de
	ld		a, (de)			// A = prev->attr
	dec		de
	cp		b				// compare with attr
	jr		z, _insert_attr_large_same_attr

	push	bc
	ld		a, 5 (ix)		// x
	push	af
	inc		sp
	push	de
	jp		_replace_attr

_insert_attr_large_same_attr:
	// ------------------------------------------------------------------------
	xor		a, a
	push	af
	inc		sp
	push	bc
	push	de
	push	de
	jp		_replace_attr_just

_insert_attr_same_x:
	// ------------------------------------------------------------------------
	// case (x == node->x)
	inc		de
	ld		a, (de)			// A = prev->attr
	cp		b				// compare with attr
	jr		z, _insert_attr_same_attr

	xor		a, a
	push	af
	inc		sp
	push	bc
	push	hl
	push	hl
	jp		_replace_attr_just

_insert_attr_same_attr:
	// ------------------------------------------------------------------------
	ld		a, #1
	push	af
	inc		sp
	push	bc
	push	hl
	dec		de
	push	de
	jp		_replace_attr_just

_insert_attr_while_break:
	// ------------------------------------------------------------------------
	inc		de
	ld		a, (de)			// A = prev->attr
	cp		b				// compare with attr
	jr		z, _insert_attr_ret

	push	de				// *1, DE = &prev->attr
	ld		d, a			// *3
	ld		l, 5 (ix)		// x
	ld		h, b			// HL = (attr, x)
	push	hl				// *2, (attr, x)

	ld		hl, #0
	ld		a, c			// rx
	cp		#80
	jr		nc, _insert_attr_last1

	// ------------------------------------------------------------------------
	// case (rx < 80)
	ld		l, c			// L = rx
	ld		h, d			// *3, H = prev->attr
	ld		de, #0
	call	_alloc_attr		// HL = new_next	// 130+17 = 147

_insert_attr_last1:
	ex		de, hl			// DE = NULL or new_next
	pop		hl				// *2, HL = (attr, x)
	call	_alloc_attr		// 130+17 = 147

	pop		de				// *1, DE = &prev->attr
	inc		de
	ex		de, hl
	ld		(hl), e
	inc		hl
	ld		(hl), d			// prev->next = new, DE = new

#ifdef CHECK_ATTR
	call	_check_attr
#endif

#ifdef COUNT_VRTC_IN_LIB
	call	_count_vrtc
#endif

	pop		ix
	ret

_insert_attr_x0:
	// ------------------------------------------------------------------------
	// case (x == 0)
	push	af				// A is already 0
	inc		sp
	push	bc
	push	hl
	push	hl
	jp		_replace_attr_just

_insert_attr_ret:
#ifdef CHECK_ATTR
	call	_check_attr
#endif

	pop		ix
#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif
__endasm;
#endif
}

//=============================================================================
#ifdef CLEAR_ATTR_FIRST
static void clear_vram_attr()
{
__asm
	ld		iy, #0
	add		iy, sp

	ld		a, #0x20
_small_beep_inst::
	//out		(0x40), a	// 0xD3 0x40 or LD A, #0x40 0x3E 0x40
	ld		a, #0x40
	// beep_on (_small_beep) = 0xd3
	// beep_off (_small_beep) = 0x3e

	ld		hl, (_OFFSCR_ADDR)
	//ld		de, #3000
	ld		de, #2960 + (NUM_LIMITED_ATTR*2)
	add		hl, de

	ld		a, #25
	ld		bc, #0xe850
	ld		de, #-BYTES_PER_LINE
	di

_clear_vram_attr_loop:
	ld		sp, hl
	.rept	(NUM_LIMITED_ATTR-1)
	push	bc
	.endm
	add		hl, de

	dec		a
	jp		nz, _clear_vram_attr_loop

	ld		sp, iy
	ei

	xor		a, a
	out		(0x40), a

	ret
__endasm;
}
#endif

//=============================================================================
#if 0 //def CLEAR_ATTR_FIRST
static void clear_vram_attr()
{
__asm
	ld		iy, #0
	add		iy, sp

	ld		a, #0x20
_small_beep_inst::
	//out		(0x40), a	// 0xD3 0x40 or LD A, #0x40 0x3E 0x40
	ld		a, #0x40
	// beep_on (_small_beep) = 0xd3
	// beep_off (_small_beep) = 0x3e

	ld		a, (_OFFSCR_ADDR+1)
	ld		h, a
	ld		l, #80

	ld		de, #0xe850
	ld		b, #25

	di

	// HL=attr top, B=loop cnt, DE=0xE850

_clear_vram_attr_loop:
	ld		a, #40
	sub		a, (hl)
	ld		c, a

	add		a, l
	ld		l, a
	adc		a, h
	sub		a, l
	ld		h, a

	ld		sp, hl

	srl		c
	ld		a, #<_clear_vram_attr_push
	sub		a, c
	ld		l, a
	ld		a, #>_clear_vram_attr_push
	sbc		a, l
	add		a, l
	ld		h, a

	jp		(hl)

	.rept	19
	push	de
	.endm

	nop
_clear_vram_attr_push:

	ld		hl, #BYTES_PER_LINE-2
	add		hl, sp

	djnz	_clear_vram_attr_loop	

	ld		sp, iy
	ei

	xor		a, a
	out		(0x40), a

	ret
__endasm;
}
#endif

#ifdef BMP_USE_INSERT_ATTR_RIGHT
//=============================================================================
void insert_attr_right(uint8_t line, uint8_t x, uint8_t attr) __naked
{
	line; x; attr;
__asm
#ifdef GREEN_MODE
	ret
#endif

	ld		iy, #0			// 14
	add		iy, sp			// 15

	ld		a, 2 (iy)		// A = line 19
	add		a, a			// 4
	add		a, a			// 4
	ld		l, a			// 4
	ld		h, #0			// 7
	ld		bc, #_mAttrList	// 10
	add		hl, bc			// HL = node = &mAttrList[line] 11

	ld		c, 3 (iy)		// 19
	ld		b, 4 (iy)		// 19
#ifdef SEMI_OFF
	ld		a, b
	and		a, #0xef
	ld		b, a
#endif
	// 126

_insert_attr_right_loop:
	ld		e, l			// 4
	ld		d, h			// DE = prev = node 4
	inc		hl				// 6
	inc		hl				// 6
	ld		a, (hl)			// 7
	inc		hl				// 6
	ld		h, (hl)			// 7
	ld		l, a			// HL = node->next 4

	//ld		a, l
	or		a, h			// 4
	jr		z, _insert_attr_right_append	// 7/12
	// 55/60

	ld		a, (hl)			// 7
	cp		c				// 4
	jr		c, _insert_attr_right_loop	// 7/12
	// 18/23

	ld		(hl), c			// 7
	inc		hl				// 6
	ld		(hl), b			// 7
	inc		hl				// 6
	xor		a, a			// 4
	ld		(hl), a			// 7
	inc		hl				// 6
	ld		(hl), a			// 7

	ret						// 10

_insert_attr_right_append:
	push	de				// 11
	ld		de, #0			// 10
	ld		l, c			// 4
	ld		h, b			// 4
	call	_alloc_attr		// 17+151
	pop		de				// 10
	ex		de, hl			// 4
	inc		hl				// 6
	inc		hl				// 6
	ld		(hl), e			// 7
	inc		hl				// 6
	ld		(Hl), d			// 7

	ret						// 10
	// 102+151 = 253
	// total(one path) = 126+60+253 = 439
__endasm;
}
#endif

//=============================================================================
#ifdef BMP_USE_IMPORT_ATTR
void import_attr(uint8_t y, uint8_t h) __naked
{
	y; h;
__asm
	ld		hl, #3
	add		hl, sp

	ld		b, (hl)		// B = h
	dec		hl
	ld		a, (hl)		// L = y
	push	af
	ld		l, a
	ld		h, #0x50
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
	// HL = list, DE = attr, B = N lines

_import_attr_loop2:
	push	bc
	push	hl
	push	de

	inc		de
	ld		a, (de)
	inc		de
	inc		hl
	ld		(hl), a
	inc		hl
	ex		de, hl
	// DE = &node->next, HL = attr+1

_import_attr_loop:
	ld		a, (hl)
	cp		#80
	jr		z, _import_attr_next

	ld		c, a
	inc		hl
	ld		a, (hl)
	inc		hl
	push	hl		// *1 next attr
	ld		l, c
	ld		h, a

	push	de		// *2 &node->next

	ld		de, #0
	call	_alloc_attr

	pop		de		// *2 &node->next
	ex		de, hl
	ld		(hl), e
	inc		hl
	ld		(hl), d
	inc		de
	inc		de		// DE = &node->next

	pop		hl		// *1 next attr
	jr		_import_attr_loop

_import_attr_next:
	pop		de
	pop		hl
	ld		bc, #4
	add		hl, bc
	ex		de, hl
	ld		bc, #BYTES_PER_LINE
	add		hl, bc
	ex		de, hl
	pop		bc
	djnz	_import_attr_loop2

	ret
__endasm;
}
#endif

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

#if 0
//=============================================================================
void append_attr(const uint8_t *table) __z88dk_fastcall __naked
{
	table;
__asm
	ld		de, #_mAttrList
	// DE = attr, HL = table

	ld		b, #25

	_append_attr_loop2:
	push	bc
	push	de
	push	hl			// *1

	_append_attr_loop1:
	ld		hl, #2
	add		hl, de
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a
	or		a, h
	ex		de, hl		// DE = next attr, HL = attr
	jr		nz, _append_attr_loop1

	// DE = null, HL = attr
	pop		de			// *1
	ld		a, (de)
	inc		de
	push	de			// *1

	push	hl
	ld		h, a
	ld		l, #60
	ld		de, #0
	call	_alloc_attr
	pop		de
	ex		de, hl
	inc		hl
	inc		hl
	ld		(hl), e
	inc		hl
	ld		(hl), d
	
	pop		de			// *1

	pop		hl
	ld		bc, #4
	add		hl, bc
	ex		de, hl

	pop		bc

	djnz	_append_attr_loop2

__endasm;
}
#endif

//=============================================================================
void insert_char_attr_reg() __naked
{
__asm
	// L = line, H = x
	// C = attr
	ld		b, c		// B = attr
	ld		c, h		// C = x
	ld		a, l
	jp		_insert_char_attr_stub
__endasm;
}

//=============================================================================
void insert_char_attr(uint8_t line, uint8_t x, uint8_t attr) __naked
{
	line; x; attr;
__asm
	ld		iy, #0
	add		iy, sp

	ld		c, 3 (iy)			// C = x
	ld		b, 4 (iy)			// B = attr
	ld		a, 2 (iy)			// A = line

_insert_char_attr_stub:
#ifdef GREEN_MODE
	ret
#endif

	add		a, a
	add		a, a
	add		a, #<_mAttrList
	ld		l, a
	adc		a, #>_mAttrList
	sub		a, l
	ld		h, a				// HL = node = &mAttrList[line]

#ifdef SEMI_OFF
	ld		a, b
	and		a, #0xef
	ld		b, a
#endif

	ld		de, #0				// DE = NULL (prev)

_insert_char_attr_loop:
	ld		a, c
	cp		(hl)				// compare x with node->x
	jr		z, _insert_char_attr_samex
	jr		c, _insert_char_attr_less
	ld		e, l
	ld		d, h				// DE = HL (prev = node)
	inc		hl
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a				// HL = node->next
	or		a, h
	jr		nz, _insert_char_attr_loop

	inc		de
	ld		a, (de)				// A = prev->attr
	cp		b
	ret		z					// ret if prev->attr == attr

_insert_char_attr_less_skip1:
	ld		a, (de)
	inc		de
	push	de				// push (&prev->next)
	push	bc
	ex		de, hl
	ld		l, c
	inc		l
	ld		h, a
	call	_alloc_attr		// (x+1, prev->attr, HL)
	pop		bc

_insert_char_attr_less_skip3:
	ex		de, hl
	ld		l, c
	ld		h, b
	call	_alloc_attr		// (x, attr, _alloc_attr)
	pop		de
	ex		de, hl
	ld		(hl), e
	inc		hl
	ld		(hl), d
	ret

_insert_char_attr_less:
	inc		de
	ld		a, (de)			// A = prev->attr
	cp		b
	ret		z				// ret if prev->attr == attr

	ld		a, c
	inc		a
	cp		(hl)			// compare (x+1) with node->x
	jr		nz, _insert_char_attr_less_skip1

	inc		hl
	ld		a, (hl)
	dec		hl
	cp		b				// compare attr with node->attr
	jr		nz, _insert_char_attr_less_skip2
	ld		(hl), c			// node->x = x
	ret

_insert_char_attr_less_skip2:
	inc		de
	push	de				// push (&prev->next)
	jr		_insert_char_attr_less_skip3

_insert_char_attr_samex:
	inc		hl
	ld		a, b
	cp		(hl)		// compare attr with node->attr
	ret		z

	inc		hl
	ld		e, (hl)
	inc		hl
	ld		d, (hl)		// DE = node->next

	inc		c			// C = x + 1 = rx

	ld		a, e
	or		a, d
	jr		z, _insert_char_attr_samex_skip1
	ld		a, (de)
	cp		c
	jr		nz, _insert_char_attr_samex_skip1

	dec		hl
	dec		hl
	ld		(hl), b		// node->attr = attr
	inc		de
	ld		a, (de)
	cp		b			// compare attr with next->attr
	ret		nz

	inc		hl
	inc		de
	ex		de, hl
	ldi					// node->next = next->next
	ldi
	ret

_insert_char_attr_samex_skip1:
	// DE = next, HL = node + 3, B = attr, C = rx
	//   node->attr = attr
	//   new(rx, node->attr, next)
	//   node->next = new
	dec		hl
	dec		hl
	ld		a, (hl)
	ld		(hl), b		// node->attr = attr
	inc		hl
	push	hl
	ld		l, c
	ld		h, a
	call	_alloc_attr
	ex		de, hl
	pop		hl
	ld		(hl), e
	inc		hl
	ld		(hl), d
	ret
__endasm;
}

#if 0
//=============================================================================
void insert_char_attr(uint8_t line, uint8_t x, uint8_t attr)
{
	AttrList_t *node, *prev, *next;
	uint8_t rx;

	node = &mAttrList[line];
	rx = x + 1;

	prev = NULL;
	while (node != NULL) {
		if (x == node->x) {
			if (node->attr == attr) return;

			next = node->next;
			if (next != NULL && rx == next->x) {
				node->attr = attr;
				if (next->attr == attr) {
					node->next = next->next;
				}
			} else {
				node->next = alloc_attr(alloc_param(rx, node->attr, next));
				node->attr = attr;
			}
			return;
		}
		else if (x < node->x) {
			if (prev->attr == attr) return;

			if (rx == node->x) {
				if (node->attr == attr) {
					node->x = x;
				} else {
					prev->next = alloc_attr(alloc_param(x, attr, node));
				}
			} else {
				next = alloc_attr(alloc_param(rx, prev->attr, node));
				prev->next = alloc_attr(alloc_param(x, attr, next));
			}
			return;
		}
		prev = node;
		node = node->next;
	}

	if (prev->attr != attr) {
		next = alloc_attr(alloc_param(rx, prev->attr, NULL));
		prev->next = alloc_attr(alloc_param(x, attr, next));
	}
}
#endif
