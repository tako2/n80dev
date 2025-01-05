#include "n80dev.h"

//=============================================================================
void init_list(ListInfo_t *info, void *list, uint8_t size, uint8_t len)
{
#if 0
	ListHeader_t *obj, *next;

	info->live = NULL;
	info->free = list;
	info->size = size;

	obj = list;
	next = (ListHeader_t *)((uint8_t *)obj + size);
	len --;
	for (; len != 0; len --) {
		obj->next = next;
		obj = next;
		next = (ListHeader_t *)((uint8_t *)next + size);
	}
	obj->next = NULL;
#else
	info; list; size; len;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)	// hl = info
	ld		e, 4 (iy)
	ld		d, 5 (iy)	// de = list
	ld		c, 6 (iy)	// c = size

	xor		a, a
	ld		(hl), a
	inc		hl
	ld		(hl), a		// *(info + #.live) = NULL
	inc		hl
	ld		(hl), e
	inc		hl
	ld		(hl), d		// *(info + #.free) = list
	inc		hl
	ld		(hl), c		// *(info + #.size) = size

	ld		b, #0
	ld		l, e
	ld		h, d
	add		hl, bc		// hl = (list + size)
	ld		a, 7 (iy)
	dec		a

	// hl = (obj + size) = next, de = obj
_init_list_loop:
	push	af

	ex		de, hl
	ld		a, c
	ld		c, #LIST_HEADER_DUMMY_SIZE
	add		hl, bc
	ld		c, a		// hl = (obj + #.next), de = next
	ld		(hl), e
	inc		hl
	ld		(hl), d
	ld		l, e
	ld		h, d
	add		hl, bc		// hl = (next + size)

	pop		af
	dec		a
	jr		nz, _init_list_loop

	ex		de, hl
	ld		c, #LIST_HEADER_DUMMY_SIZE
	add		hl, bc
	xor		a, a
	ld		(hl), a
	inc		hl
	ld		(hl), a		// *(obj + #.next) = NULL

__endasm;
#endif
}

#ifndef LIST_ADD_ALLOC_NODE_LAST
//=============================================================================
void *alloc_list(ListInfo_t *info) __z88dk_fastcall __naked
{
	info;
__asm
	ld		iy, #0
	add		iy, sp

	di
	ld		sp, hl
	pop		de	// de = live_top (info->live)
	pop		hl	// hl = free_top = obj (info->free)
	ld		a, l
	or		a, h
#ifdef LIST_USE_ASSERT_NULL
	jr		z, _alloc_list_halt
#else
	jr		z, _alloc_list_ret
#endif

	ld		bc, #LIST_HEADER_DUMMY_SIZE+1
	add		hl, bc	// hl = &obj->next + 1
	ld		b, (hl)
	dec		hl
	ld		c, (hl)	// bc = obj->next
	push	bc		// info->free = obj->next
	ld		(hl), e
	inc		hl
	ld		(hl), d	// obj->next = info->live
	ld		bc, #-(LIST_HEADER_DUMMY_SIZE+1)
	add		hl, bc
	push	hl		// info->live = obj

_alloc_list_ret:
	ld		sp, iy
	ei
	ret

#ifdef LIST_USE_ASSERT_NULL
_alloc_list_halt:
	halt
#endif
__endasm;

#if 0
	ListHeader_t *obj;

	if (info->free == NULL) return (NULL);

	obj = info->free;
	info->free = obj->next;

	obj->next = info->live;
	info->live = obj;

	return (obj);
#endif
}
#else
//=============================================================================
void *alloc_list(ListInfo_t *info) __z88dk_fastcall __naked
{
	info;
__asm
	ld		iy, #0
	add		iy, sp

	di
	ld		sp, hl
	pop		de	// DE = live_top (info->live)
	pop		hl	// HL = free_top = obj (info->free)
	ld		a, l
	or		a, h
#ifdef LIST_USE_ASSERT_NULL
	jr		z, _alloc_list_last_halt
#else
	jr		z, _alloc_list_last_ret
#endif

	ld		bc, #LIST_HEADER_DUMMY_SIZE
	add		hl, bc	// HL = &obj->next + 1
	ld		c, (hl)
	inc		hl
	ld		b, (hl)	// BC = obj->next
	push	bc		// info->free = obj->next

	xor		a, a
	ld		(hl), a
	dec		hl
	ld		(hl), a	// obj->next = NULL
	ld		bc, #LIST_HEADER_DUMMY_SIZE
	sbc		hl, bc	// HL = obj

	ld		(_alloc_list_last_retval+1), hl

_alloc_list_last_loop:
	ld		a, d
	or		a, e
	jr		z, _alloc_list_last_retval

	ex		de, hl
	add		hl, bc
	ld		sp, hl	// SP = live->next
	pop		de

	jr		_alloc_list_last_loop

_alloc_list_last_retval:
	ld		hl, #0
	push	hl		// live->next = obj

_alloc_list_last_ret:
	ld		sp, iy
	ei
	ret

#ifdef LIST_USE_ASSERT_NULL
_alloc_list_last_halt:
	halt
#endif
__endasm;
}
#endif

//=============================================================================
void free_list(ListInfo_t *info, ListHeader_t *obj)
{
	info; obj;
#if 0
	ListHeader_t *ptr, *obj_next;

	obj_next = obj->next;

	obj->next = info->free;
	info->free = obj;

	if (obj == info->live) {
		info->live = obj_next;
	} else {
		ptr = info->live;
		while (1 /* ptr->next != NULL */) {
			if (obj == ptr->next) {
				ptr->next = obj_next;
				break;
			}
			ptr = ptr->next;
		}
	}
#else
__asm
	ld		iy, #0
	add		iy, sp

	di
	pop		af		// skip return address
	pop		de		// DE = info
	pop		hl		// HL = obj

	ld		(_obj_work+1), hl
	ex		de, hl
	// HL = info, DE = obj

	ld		sp, hl
	pop		bc		// BC = info->live
	pop		hl		// HL = info->free
	push	de		// info->free = obj

	ld		a, #LIST_HEADER_DUMMY_SIZE
	add		a, e
	ld		e, a
	adc		a, d
	sub		a, e
	ld		d, a	// DE = &obj->next

	ex		de, hl	// HL = &obj->next, DE = prev info->free
	ld		sp, hl
	pop		hl		// HL = obj->next (obj_next)
	push	de		// obj->next = prev info->free

	ld		sp, iy
	ei

	push	hl		// *1 (obj_next)

_obj_work:
	ld		hl, #0	// HL = obj
	ld		e, l
	ld		d, h	// DE = HL = obj
	cp		a, a
	sbc		hl, bc
	jr		nz, _not_first	// compare HL(obj) with BC(info->live)

	pop		de		// *1 (obj_next)

	// info->live = obj_next
	ld		l, 2 (iy)
	ld		h, 3 (iy)
	ld		(hl), e
	inc		hl
	ld		(hl), d

	ret

_not_first:
	// DE = obj, BC = info->live(ptr)
	ld		l, c
	ld		h, b	// HL = ptr
	ld		c, e
	ld		b, d	// BC = obj
	ex		de, hl	// DE = ptr
	// DE = ptr, BC = obj

_free_list_loop:
	ld		hl, #LIST_HEADER_DUMMY_SIZE+1
	add		hl, de	// HL = &ptr->next+1

	ld		d, (hl)
	dec		hl
	ld		e, (hl)	// DE = ptr->next
	ld		a, c
	cp		e
	jr		nz, _free_list_loop
	ld		a, b
	cp		d
	jr		nz, _free_list_loop

	pop		de		// *1 (obj_next)
	ld		(hl), e
	inc		hl
	ld		(hl), d	// ptr->next = obj_next

	ret
__endasm;
#endif
}
