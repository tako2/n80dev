#include "n80dev.h"

//#define CHECK_ATTR

//#define SEMI_OFF
//#define GREEN_MODE

#define NUM_LIMITED_ATTR 5

//#define CHECK_FREE_ATTR

AttrList_t mAttrList[25];

AttrList_t *mFreeAttr;
//#define LARGE_ATTR_WORK
#define NUM_ATTR_LIST_WORK 256
AttrList_t mAttrListWork[NUM_ATTR_LIST_WORK];

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
	ret
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
	jp		_count_vrtc
#else
	ret
#endif
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
	ld		c, l	// C = x
	ld		b, h	// B = attr

#ifdef CHECK_FREE_ATTR
#ifndef LARGE_ATTR_WORK
	ld		hl, #_num_free_attr
	dec		(hl)
	jr		z, _alloc_attr_skip
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

//=============================================================================
void add_node_rx_prev_attr() __naked
{
__asm
	// HL = node, DE = prev, C = rx
	ex		de, hl		// DE = node, HL = prev
	inc		hl
	ld		h, (hl)		// H = prev->attr
	ld		l, c		// L = rx
	jp		_alloc_attr	// HL = new node (rx, prev->attr, node)
__endasm;
}

//=============================================================================
void alloc_and_add_node_to_start() __naked
{
__asm
	ld		c, 3 (iy)	// C = x
	ld		b, 5 (iy)	// B = attr
	// C = x, B = attr, DE = next
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
__endasm;
}

//=============================================================================
void add_node_to_start() __naked
{
__asm
	// HL = node
	ex		de, hl		// DE = node
_add_prev_to_start::
	pop		hl			// HL = start
_add_prev_to_node::
	inc		hl
	inc		hl
	ld		(hl), e
	inc		hl
	ld		(hl), d		// start->node = new node

#ifdef CHECK_ATTR
	call	_check_attr
#endif

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif
__endasm;
}

//=============================================================================
void add_attr_node_to_start() __naked
{
__asm
	// HL = node, DE = dont care
	ex		de, hl		// DE = node
	pop		hl			// HL = start
	inc		hl
	ld		a, 5 (iy)	// A = attr
	ld		(hl), a
	inc		hl
	ld		(hl), e
	inc		hl
	ld		(hl), d		// start->node = new node

#ifdef CHECK_ATTR
	call	_check_attr
#endif

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif
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
void replace_attr() __naked
{
__asm
	// HL = start, C = rx

	ld		b, #0		// B = N
	push	hl			// push start

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
	jr		z, _replace_attr_null

	ld		a, c
	cp		(hl)		// compare rx with node->x
	jr		z, _replace_attr_same_x		// if (rx == node->x)
	jr		c, _replace_attr_large		// if (rx < node->x)

	inc		b
	jp		_replace_attr_loop

_replace_attr_same_x:
	ld		a, 5 (iy)	// A = attr
	inc		hl
	cp		(hl)		// compare attr with node->attr
	dec		hl
	jr		nz, _replace_attr_same_x_skip1	// if (attr != node->attr)

	// attr == node->attr
	ld		e, l
	ld		d, h		// DE = node
	inc		hl
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a		// prev = node, node = node->next
	inc		b

_replace_attr_same_x_skip1:
	ld		a, b
	and		a, a
	jr		nz, _replace_attr_link_prev		// if (n != 0)

	// n == 0
	ex		de, hl		// DE = node
	//ld		c, 3 (iy)	// C = x
	//ld		b, 5 (iy)	// B = attr
	jp		_alloc_and_add_node_to_start
	/*** return ***/

_replace_attr_null:
	// node == NULL
	ld		a, c
	cp		#80
	jr		nc, _replace_attr_link_prev

_replace_attr_large:
	// HL = node, DE = prev
	inc		de
	ld		a, (de)		// A = prev->attr
	dec		de
	cp		5 (iy)
	jr		z, _replace_attr_link_prev	// if (attr == prev->attr)

	ld		a, b
	sub		a, #1

	jr		z, _replace_attr_large_ins_1
	jr		c, _replace_attr_large_ins_2

	// default (n >= 2)
	pop		hl			// HL = start

	inc		hl
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a		// HL = start->next

	ld		a, 3 (iy)
	ld		(hl), a		// start->next->x = x
	inc		hl

	ld		a, 5 (iy)
	ld		(hl), a		// start->next->attr = attr
	inc		hl

	ld		(hl), e
	inc		hl
	ld		(hl), d		// start->next->next = prev

	ex		de, hl
	ld		(hl), c		// prev->x = rx

#ifdef CHECK_ATTR
	call	_check_attr
#endif

#ifdef COUNT_VRTC_IN_LIB
	jp		_count_vrtc
#else
	ret
#endif

_replace_attr_link_prev:
	// n != 0
	ld		a, 3 (iy)
	ld		(de), a		// prev->x = x
	inc		de
	ld		a, 5 (iy)
	ld		(de), a		// prev->attr = attr
	dec		de
	jp		_add_prev_to_start
	/*** return ***/

_replace_attr_large_ins_1:
	ld		a, c
	ld		(de), a		// prev->x = rx

	// DE = prev
	//ld		c, 3 (iy)	// x
	//ld		b, 5 (iy)	// attr
	jp		_alloc_and_add_node_to_start
	/*** return ***/

_replace_attr_large_ins_2:
	ex		de, hl		// DE = node
	inc		hl
	ld		h, (hl)		// H = start->attr
	ld		l, c		// L = rx
	call	_alloc_attr

	ex		de, hl		// DE = next
	//ld		c, 3 (iy)	// L = x
	//ld		b, 5 (iy)	// H = attr
	jp		_alloc_and_add_node_to_start
	/*** return ***/

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
void replace_attr_just() __naked
{
__asm
	// HL = prev, C = rx, B = n
	// push (start)

_replace_attr_just_loop:
	ld		e, l
	ld		d, h
	inc		hl
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a		// HL = node->next, DE = node(as prev)

	or		a, h
	jr		z, _replace_attr_just_null		// if (node == NULL)

	ld		a, c
	cp		(hl)		// compare rx with node->x
	jr		z, _replace_attr_just_same_x	// if (rx == node->x)
	jr		c, _replace_attr_just_find		// if (rx < node->x)

	inc		b
	jp		_replace_attr_just_loop

_replace_attr_just_same_x:
	ld		a, 5 (iy)	// A = attr
	inc		hl
	cp		(hl)		// compare with node->attr
	dec		hl			// HL = node
	jr		nz, _replace_attr_just_same_x_skip	// if (attr != node->attr)
	inc		hl
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a		// HL = node->next
_replace_attr_just_same_x_skip:
	jp		_add_attr_node_to_start

_replace_attr_just_null:
	// node == NULL
	// HL = null, DE = prev
	ld		a, c
	cp		#80
	jp		nc, _add_attr_node_to_start	// HL = NULL, DE = prev
	// rx < 80
_replace_attr_just_find:
	// rx < node->x
	ld		a, 5 (iy)	// A = attr
	ex		de, hl		// HL = prev, DE = node
	inc		hl
	cp		(hl)
	jr		z, _replace_attr_just_large_skip	// if (attr == prev->attr)

	// attr != prev->attr
	ld		a, b
	and		a, a
	jr		nz, _replace_attr_just_large_skip2	// if (n != 0)

	// n == 0
	// DE = node
	ld		h, (hl)		// H = prev->attr
	ld		l, c		// L = rx
	call	_alloc_attr
	jp		_add_attr_node_to_start
	/*** return ***/

_replace_attr_just_large_skip2:
	// n != 0
	dec		hl
	ld		(hl), c		// prev->x = rx
	jp		_add_attr_node_to_start

_replace_attr_just_large_skip:
	ex		de, hl		// HL = node or null
	jp		_add_attr_node_to_start
	/*** return ***/

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
		// start = node, prev = node, n = 0
		replace_attr_just(node, node, rx, attr, 0);
		assert(node->next != node);
		return;
	}

	prev = node;
	node = node->next;
	while (node != NULL) {
		if (x == node->x) {
			if (attr != prev->attr) {
				// start = node, prev = node, n = 0
				replace_attr_just(node, node, rx, attr, 0);
			} else {
				// start = prev, prev = node, n = 1
				// x = prev->x;
				replace_attr_just(prev, node, rx, attr, 1);
			}
			assert(node->next != node);
			return;
		}
		else if (x < node->x) {
			if (attr != prev->attr) {
				// start = prev
				replace_attr(prev, x, rx, attr);
			} else {
				// start = prev, prev = prev, n = 0
				// x = prev->x;
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
	ld		iy, #0
	add		iy, sp

	ld		a, 2 (iy)		// line
	add		a, a
	add		a, a
	add		a, #<_mAttrList
	ld		l, a
	adc		a, #>_mAttrList
	sub		a, l
	ld		h, a			// HL = node = &mAttrList[line]

	ld		b, 3 (iy)		// B = x
	ld		a, 4 (iy)
	add		a, b
	ld		c, a			// C = rx (= x + w)

	ld		a, b
	and		a, a
	jp		nz, _insert_attr_loop	// if (x != 0)

	// ------------------------------------------------------------------------
	// case (x == 0)
	ld		b, #0
	push	hl
	jp		_replace_attr_just

	// ------------------------------------------------------------------------
_insert_attr_loop:
	ld		e, l
	ld		d, h			// DE = node
	inc		hl
	inc		hl
	ld		a, (hl)
	inc		hl
	ld		h, (hl)
	ld		l, a			// HL = node->next

	or		a, h
	jr		z, _insert_attr_null

	ld		a, b
	cp		(hl)
	jr		z, _insert_attr_same
	jr		nc, _insert_attr_loop

	// ------------------------------------------------------------------------
	// case (x < node->x)
	inc		de
	ld		a, (de)			// A = prev->attr
	dec		de
	cp		5 (iy)			// compare with attr
	jr		z, _insert_attr_find_skip

	// prev->attr != attr
	ex		de, hl			// HL = prev
	jp		_replace_attr
	/*** return ***/

_insert_attr_find_skip:
	// prev->attr == attr
	ld		b, #0
	push	de				// push prev as start
	ex		de, hl
	jp		_replace_attr_just
	/*** return ***/

_insert_attr_same:
	// ------------------------------------------------------------------------
	// case (x == node->x)
	inc		de
	ld		a, (de)			// A = prev->attr
	dec		de
	cp		5 (iy)			// compare with attr
	jr		z, _insert_attr_same_skip	// if (prev->attr == attr)

	// attr != prev->attr
	ld		b, #0		// n = 0
	push	hl			// push node as start
	jp		_replace_attr_just
	/*** return ***/

_insert_attr_same_skip:
	// attr == prev->attr
	ld		b, #1		// n = 1
	push	de			// push prev as start
	jp		_replace_attr_just
	/*** return ***/

_insert_attr_null:
	// ------------------------------------------------------------------------
	// node == NULL
	// HL = null, DE = prev
	inc		de
	ld		a, (de)			// A = prev->attr
	cp		5 (iy)			// compare with attr
	ret		z

	dec		de
	push	de				// push prev as start

	ld		b, a
	ld		a, c
	cp		#80
	jr		nc, _insert_attr_null_skip

	// rx < 80
	ex		de, hl			// DE = null
	ld		h, b			// H = prev->attr
	ld		l, c			// L = rx
	call	_alloc_attr		// HL = new node

_insert_attr_null_skip:
	// HL = null or new node
	ex		de, hl
	//ld		c, 3 (iy)		// L = x
	//ld		b, 5 (iy)		// H = attr
	jp		_alloc_and_add_node_to_start
	/*** return ***/
__endasm;
#endif
}

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
	ld		bc, #120
	add		hl, bc
	ex		de, hl
	pop		bc
	djnz	_import_attr_loop2

	ret
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
	ld		hl, #2
	add		hl, sp

	ld		a, (hl)		// A = line
	inc		hl
	ld		c, (hl)		// C = x
	inc		hl
	ld		b, (hl)		// B = attr

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
	jr		z, _insert_char_attr_samex	// if (x == node->x)
	jr		c, _insert_char_attr_less	// if (x < node->x)

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

	// node == NULL
	inc		de
	ld		a, (de)				// A = prev->attr
	cp		b
	ret		z					// ret if prev->attr == attr

	// HL = null, DE = prev+1

_insert_char_attr_less_skip1:
	ld		a, (de)			// A = prev->attr
	inc		de
	push	de				// push (&prev->next)
	push	bc
	ex		de, hl			// DE = prev HL
	ld		l, c
	inc		l				// L = x + 1 (=rx)
	ld		h, a			// H = prev->attr
	call	_alloc_attr		// (x+1, prev->attr, HL)
	pop		bc

_insert_char_attr_less_skip3:
	ex		de, hl
	ld		l, c			// C = x
	ld		h, b			// B = attr
	call	_alloc_attr		// (x, attr, _alloc_attr)

	pop		de				// DE = &prev->next, HL = new node2
	ex		de, hl
	ld		(hl), e
	inc		hl
	ld		(hl), d			// prev->next = new node2
	ret

	//-------------------------------------------------------------------------
	// x < node->x
_insert_char_attr_less:
	inc		de
	ld		a, (de)			// A = prev->attr
	cp		b
	ret		z				// ret if (prev->attr == attr)

	ld		a, c
	inc		a				// A = x + 1
	cp		(hl)			// compare (x+1) with node->x
	jr		nz, _insert_char_attr_less_skip1	// if (rx != node->x)

	// rx == node->x
	inc		hl
	ld		a, (hl)			// A = node->attr
	dec		hl
	cp		b				// compare attr with node->attr
	jr		nz, _insert_char_attr_less_skip2	// if (node->attr != attr)

	ld		(hl), c			// node->x = x
	ret

_insert_char_attr_less_skip2:
	// node->attr != attr
	inc		de
	push	de				// push (&prev->next)
	jr		_insert_char_attr_less_skip3

_insert_char_attr_samex:
	// x == node->x
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
	jr		z, _insert_char_attr_samex_skip1	// if (next == NULL)
	ld		a, (de)
	cp		c
	jr		nz, _insert_char_attr_samex_skip1	// if (next->x != rx)

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
