#include "n80dev.h"

//#define CHECK_ATTR

#define NO_ATTR 0x18

//=============================================================================
void right_shift_attr(uint8_t line, uint8_t l_idx, int8_t shift) __naked
{
	line; l_idx; shift;
__asm
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, #(BYTES_PER_LINE - 1)	//119
	call	_get_offscr_addr

	ld		a, 3 (iy)
	ld		c, 4 (iy)
	add		a, c      // A = l_idx + shift
	ret		m
	ret		z

	ld		b, #0xff
	ld		d, h
	ld		e, l
	add		hl, bc
	add		hl, bc

	// HL : SRC
	// DE : DEST
	// A  : LEN

	add		a, a
	ld		c, a
	ld		b, #0
	lddr

	ret
__endasm;

#if 0
	uint8_t *dest, *src;
	int8_t len;

	dest = get_offscr_addr(line | (119 << 8));
	src = dest + shift * 2;

#if 0
	len = 19 + shift - l_idx;
#else
	len = l_idx + shift;
#endif
	if (len <= 0) {
		return;
	}
	for (; len > 0; len --) {
		if (*src != NO_ATTR) break;
		dest -= 2;
		src -= 2;
	}
	for (; len > 0; len --) {
		*dest -- = *src --;
		*dest -- = *src --;
	}
#endif
}

//=============================================================================
void left_shift_attr(uint8_t *dest, uint8_t l_idx, int8_t shift) __naked
{
	dest; l_idx; shift;
__asm
	ld		iy, #0
	add		iy, sp

	ld		a, 4 (iy) // A = l_idx
	ld		c, 5 (iy) // C = shift

	sub		a, c // A = l_idx - shift
	ret		m
	ret		z

	ld		l, 2 (iy)
	ld		h, 3 (iy)
	inc		hl
	inc		hl
	ld		d, h
	ld		e, l

	ld		b, #0
	add		hl, bc
	add		hl, bc

	// HL : SRC
	// DE : DEST

	add		a, a
	ld		c, a
	ld		b, #0

	ldir

	ex		de, hl

	ld		b, 5 (iy)
_left_shift_attr_1:
	ld		(hl), #80
	inc		hl
	ld		(hl), #NO_ATTR // 0xe8
	inc		hl
	djnz	_left_shift_attr_1

	ret
__endasm;

#if 0
	uint8_t *src;
	int8_t len;

	dest += 2;
	src = dest + shift * 2;

#if 0
	len = 19 - shift - l_idx;
#else
	len = l_idx - shift;
#endif
	if (len <= 0) {
		return;
	}
	for (; len > 0; len --) {
		if (*src == 80) break;
		*dest ++ = *src ++;
		*dest ++ = *src ++;
	}
	shift += len;
	for (; shift > 0; shift --) {
		*dest ++ = 80;
		*dest ++ = NO_ATTR;
	}
#endif
}

#ifdef CHECK_ATTR
//=============================================================================
uint8_t check_attr(uint8_t line) __z88dk_fastcall __naked
{
	line;
__asm
	//  AF: (broken)
	//	BC: (broken)
	//  DE: (broken)
	ld		h, #80
	call	_get_offscr_addr

	ld		a, (hl)
	and		a, a
	jr		nz, _check_attr_fail

	inc		hl
	inc		hl

	ld		c, #0
	ld		d, #0
	ld		b, #ATTRS_PER_LINE-1	//19
_check_attr_1:
	ld		a, (hl)
	inc		hl
	cp		c
	jr		c, _check_attr_fail
	ld		c, a
	ld		a, (hl)
	inc		hl
	cp		#NO_ATTR // 0xe8
	jr		z, _check_attr_2
	cp		d
	jr		z, _check_attr_fail
_check_attr_2:
	ld		d, a
	djnz	_check_attr_1

	ld		l, #1
	ret

_check_attr_fail:
	ld		l, #0
	ret
__endasm;
}
#endif

//=============================================================================
void insert_attr(uint8_t line, uint8_t x, uint8_t w, uint8_t attr) __naked
{
	line; x; w; attr;
__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		l, 4 (ix)
	ld		h, #80
	call	_get_offscr_addr // HL = attr_ptr

	inc		hl
	ld		a, (hl)
	cp		#NO_ATTR // 0xe8
	jr		z, _insert_attr_ret0

	ld		a, 5 (ix) // A = x as L_POS
	inc		hl
	ld		b, #ATTRS_PER_LINE-1	//19    // B as L_IDX
	ld		d, #0     // D as FLAG
_insert_attr_1:
	ld		c, (hl)   // C = ptr->pos
	cp		c         // (l_pos - ptr->pos) < 0
	jr		c, _insert_attr_2
	inc		hl
	inc		hl
	djnz	_insert_attr_1
	pop		ix
	ret

_insert_attr_ret0:
	ld		a, 7 (ix)
	ld		(hl), a
	pop		ix
	ret

_insert_attr_2:
	dec		hl
	ld		e, (hl) // E = ptr->attr
	dec		hl
	ld		c, (hl) // C = ptr->pos
	cp		c
	jr		z, _insert_attr_3

	// L_POS != ptr->pos
	ld		a, 7 (ix) // A = attr
	cp		e
	jr		z, _insert_attr_4

	// attr != ptr->attr
	inc		d // D = 1
	jr		_insert_attr_4

_insert_attr_3:
	// L_POS == ptr->pos
	ld		a, #ATTRS_PER_LINE-1	//19
	cp		b
	jr		z, _insert_attr_4

	// L_IDX < 19
	dec		hl
	ld		e, (hl) // E = (ptr-1)->attr
	inc		hl
	ld		a, 7 (ix) // A = attr
	cp		e
	jr		nz, _insert_attr_4

	// attr == (ptr-1)->attr
	inc		b
	dec		hl
	dec		hl

_insert_attr_4:
	// exit first loop
	// D : FLAG
	push	hl // push L_PTR
	push	bc // push L_IDX

	inc		hl
	inc		hl

	ld		a, 5 (ix)
	add		a, 6 (ix) // A = x + w as R_POS

_insert_attr_5:
	ld		c, (hl)
	cp		c
	jr		c, _insert_attr_6

	jr		z, _insert_attr_7	// R_POS == ptr->pos

	inc		hl
	inc		hl
	djnz	_insert_attr_5
#if 0
	dec		hl
	dec		hl
#endif
	jr		_insert_attr_7

_insert_attr_6:
	// R_POS < ptr->pos
	dec		hl
	ld		e, (hl) // E = ptr->attr
	dec		hl
	ld		c, (hl) // C = ptr->pos
	cp		c
	jr		z, _insert_attr_8

	// R_POS != ptr->pos
	ld		a, 7 (ix) // A = attr
	cp		e
	jr		z, _insert_attr_7

	// attr != ptr->attr
	ld		a, #2
	or		a, d
	ld		d, a // D |= 2
	// E : R_ATTR
	jr		_insert_attr_7

_insert_attr_8:
	// R_POS == ptr->pos
	ld		a, 7 (ix) // A = attr
	cp		e
	jr		z, _insert_attr_7

	// attr != ptr->attr
	inc		b
#if 0
	dec		hl
	dec		hl
#endif

	// D : FLAG
	// E : R_ATTR
	// B : R_IDX
	// exit second loop
_insert_attr_7:
	ld		a, b  // A = R_IDX
	pop		bc    // B = L_IDX
	pop		hl    // HL = L_PTR
	push	hl
	ld		c, d  // C = FLAG
	push	bc
	sub		a, b  // A = R_IDX - L_IDX
	neg
	rrc		d
	jr		nc, _insert_attr_c1
	dec		a
_insert_attr_c1:
	rrc		d
	jr		nc, _insert_attr_c2
	dec		a
_insert_attr_c2:
	and		a, a
	jr		z, _insert_attr_c3
	jp		p, _insert_attr_c4

	// shift < 0
	// shift right
	push	de
	push	af
	inc		sp
	ld		c, 4 (ix)
	push	bc
	call	_right_shift_attr
	pop		af
	inc		sp
	pop		de
	jr		_insert_attr_c3

_insert_attr_c4:
	// shift > 0
	// shift left
	push	de
	ld		c, b // C = L_IDX
	ld		b, a // B = SHIFT
	push	bc
	push	hl
	call	_left_shift_attr
	pop		af
	pop		af
	pop		de

_insert_attr_c3:
	pop		bc // B = 19 - L_IDX, C = FLAG
	pop		hl // HL = L_PTR

	rrc		c
	jr		nc, _insert_attr_c5
	xor		a, a
	cp		b
	jr		z, _insert_attr_c5

	// (flag & 1) != 0 && rem != 0
	dec		b
	inc		hl
	inc		hl
	ld		a, 5 (ix)
	ld		(hl), a
_insert_attr_c5:
	inc		hl
	ld		a, 7 (ix)
	ld		(hl), a
	dec		hl

	rrc		c
	jr		nc, _insert_attr_c7
	xor		a, a
	cp		b
	jr		z, _insert_attr_c7

	// (flag & 2) != 0 && rem != 0
	inc		hl
	inc		hl
	ld		a, 5 (ix)
	add		a, 6 (ix) // A = x + w as R_POS
	ld		(hl), a
	inc		hl
	ld		(hl), e

_insert_attr_c7:

#ifdef CHECK_ATTR
	ld		l, 4 (ix)
	call	_check_attr
	ld		a, l
	and		a, a
	jr		nz, _check_attr_ok_1
	halt
	_check_attr_ok_1:
#endif

_insert_attr_ret:
	pop		ix
	ret
__endasm;

#if 0
#ifdef CHECK_ATTR
	check_attr(line);
#endif
#endif
}

//=============================================================================
#define VRAM_SIZE (BYTES_PER_LINE * 25)
void clear_attr(uint8_t attr) __z88dk_fastcall __naked
{
	attr;
__asm
	ld		a, (_OFFSCR_ADDR+1)
	add		a, #(VRAM_SIZE >> 8)	// #0xb
	ld		h, a
	ld		b, l
	ld		l, #(VRAM_SIZE & 0xff)	// #0xb8	// OFFSCR_ADDR + 120 * 25
	ld		c, #0		// BC = attr | 0x00
	ld		de, #0xe850

	ld		iy, #0
	add		iy, sp

	ld		a, #25

	di
_clear_attr_loop:
	ld		sp, hl
	.rept	(ATTRS_PER_LINE-1)	// 19
	push	de
	.endm
	push	bc
	ex		af, af
	ld		a, l
	sub		a, #BYTES_PER_LINE
	ld		l, a
	sbc		a, a
	add		a, h
	ld		h, a
	ex		af, af
	dec		a
	jr		nz, _clear_attr_loop
	ld		sp, iy
	ei

	ret
__endasm;
}

//=============================================================================
#ifdef BMP_USE_CLEAR_ATTR_MAPPED
void clear_attr_mapped(uint8_t start_line, uint8_t num_lines,
					   const uint8_t *attr_map,
					   uint8_t wait, uint8_t cycle) __naked
{
	start_line; num_lines; attr_map; wait; cycle;

__asm
	push	ix
	ld		ix, #0
	add		ix, sp

	ld		a, #0x20
	ld		(_clear_attr_mapped_work), a

	ld		b, 8 (ix)	// wait
	ld		c, 9 (ix)	// cycle
	push	bc

	ld		l, 4 (ix)
	ld		h, #80
    call    _get_offscr_addr

	ld		e, 6 (ix)
	ld		d, 7 (ix)
_clear_attr_mapped_1:
	ex		de, hl		// 4
	ld		a, (hl)		// 7
	inc		hl			// 6
	ex		de, hl		// 4
	push	de			// 11

	ld		(hl), #0	// 10
	inc		hl			// 6
	ld		(hl), a		// 7
	inc		hl			// 6
	ld		(hl), #80	// 10
	inc		hl			// 6
	ld		(hl), #NO_ATTR // 0xe8	// 10
	ld		e, l		// 4
	ld		d, h		// 4
	inc		de			// 6
	dec		hl			// 6
	ld		bc, #40-4	// 10
#if 0
	ldir				// 21/16
#else
	ldi					// 16
	ldi					// 16
	ldi					// 16
	ldi					// 16
_clear_attr_mapped_ldi:
	ldi					// 16
	ldi					// 16
	ldi					// 16
	ldi					// 16
	ldi					// 16
	ldi					// 16
	ldi					// 16
	ldi					// 16
	jp		pe, _clear_attr_mapped_ldi // 10
	// 616
#endif
	ld		hl, #80		// 10
	add		hl, de		// 11

	pop		de			// 10

	pop		bc			// 10
	ld		a, c		// 4

	and		a, a		// 4
	jr		z, _clear_attr_mapped_2	// 12/7
	dec		c			// 4

	ld		a, b		// 4
_clear_attr_mapped_3:
	djnz	_clear_attr_mapped_3	// 13/8
	ld		b, a		// 4

	ld		a, (_clear_attr_mapped_work)	// 13
	out		(0x40), a	// 11
	xor		a, #0x20	// 7
	ld		(_clear_attr_mapped_work), a	// 13

_clear_attr_mapped_2:
	push	bc			// 11
	dec		5 (ix)		// 23
						// 260 + 11 + (4+12)(N-1) + LDIR(751) <- obs
						// 117 + LDIR(616) + 39 + 7 + 8 + DJNZ((N-1)*13+8) + 82
	// DJNZ(13 * (N-1) + 8) + 869
	jr		nz, _clear_attr_mapped_1	// 12/7

	pop		bc			// 10

_clear_attr_mapped_6:
	ld		a, c		// 4
	and		a, a		// 4
	jr		z, _clear_attr_mapped_4	// 12/7
	dec		c			// 4

	ld		a, b		// 4
	// 158 + LDIR(751) = 909 wait => 69 <- obs
	// 869 wait => 67
	ld		b, #67		// 7
_clear_attr_mapped_7:
	djnz	_clear_attr_mapped_7	// 13/8
	ld		b, a		// 4
_clear_attr_mapped_5:
	djnz	_clear_attr_mapped_5	// 13/8
	ld		b, a		// 4

	ld		a, (_clear_attr_mapped_work)	// 13
	out		(0x40), a	// 11
	xor		a, #0x20	// 7
	ld		(_clear_attr_mapped_work), a	// 13
	jr		_clear_attr_mapped_6	// 12

_clear_attr_mapped_4:

	pop		ix
	ret

_clear_attr_mapped_work:
	.ds		1
__endasm;
}
#endif
