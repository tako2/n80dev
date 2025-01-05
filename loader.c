#include "n80dev.h"

//#define LOAD_FROM_SD

#define CMT_OPEN_R	0x0bf3
//#define CMT_READ	0x5f9e <= maybe 0x5f3a
#ifndef LOAD_FROM_SD
#define CMT_READ	0x0c88
#else
#define CMT_READ	0x6006
#endif
#define CMT_CLOSE	0x0c2e


//=============================================================================
static void put_bmp() __naked
{
__asm
	// HL = src
	// DE = dest
	// B = h
	// C = w
_put_bmp_loop:
	push	bc
	ld		b, #0
	push	de
	ldir
	pop		de
	call	__next_de
	pop		bc
	djnz	_put_bmp_loop
	ret

__next_de:
	ld		a, #120
	add		a, e
	ld		e, a
	adc		a, d
	sub		a, e
	ld		d, a
	ret
__endasm;
}

//=============================================================================
static void load_attrs() __naked
{
__asm
	ld		de, #0xf300+81
	ld		b, #25

_load_attrs_loop:
	ld		a, (hl)
	inc		hl
	or		a, a
	jr		z, _load_attrs_skip

	push	de
	push	bc
	ld		c, a
	ld		b, #0
	ldir
	ld		a, #80
	ld		(de), a
	pop		bc
	pop		de

_load_attrs_skip:
	call	__next_de
	djnz	_load_attrs_loop

	ret
__endasm;
}

//=============================================================================
void draw_scr(uint8_t *data) __z88dk_fastcall __naked
{
	data;
__asm
	push	hl

	call	0x0bd2	// Cursor Off
	ld		bc, #0x00ff
	call	0x08f7	// Color
	ld		bc, #80 * 256 + 25
	call	0x093a	// Width x Height

	pop		hl

_draw_scr_loop:
	ld		e, (hl)
	inc		hl
	ld		d, (hl)
	inc		hl
	ld		a, d
	or		a, e
	jr		z, _load_attrs

	ld		c, (hl)
	inc		hl
	ld		b, (hl)
	inc		hl
	call	_put_bmp
	jr		_draw_scr_loop
__endasm;
}

//=============================================================================
void chk_progbar() __naked
{
__asm
	// HL = table
	// A = chunk no
	// DE = attr

	cp		(hl)
	ret		c
	inc		hl
	ex		de, hl
	inc		(hl)
	ex		de, hl
	jr		_chk_progbar
	//ret
__endasm;
}

//=============================================================================
static void skip_3a() __naked
{
__asm
_skip_3a_loop:
	call	CMT_READ
	cp		#0x3a
	jr		nz, _skip_3a_loop
	ret
__endasm;
}

//=============================================================================
void load_bin(const uint8_t *table) __z88dk_fastcall __naked
{
	table;
__asm
	ld		e, (hl)
	inc		hl
	ld		d, (hl)
	inc		hl
	xor		a, a
	exx
	ex		af, af

#if 0
	ld		a, #0x29
	out		(0x30), a	// Motor On
#endif

#ifndef LOAD_FROM_SD
	call	CMT_OPEN_R
#endif

	call	_skip_3a

	call	CMT_READ
	ld		h, a
	call	CMT_READ
	ld		l, a
	call	CMT_READ
	add		a, h
	add		a, l
	jr		nz, _load_bin_exit_err

_load_bin_loop2:
	call	_skip_3a

	call	CMT_READ
	or		a, a
	jr		z, _load_bin_end
	ld		b, a
	ld		c, a

_load_bin_loop1:
	call	CMT_READ
	ld		(hl), a
	inc		hl
	add		a, c
	ld		c, a
	djnz	_load_bin_loop1

	call	CMT_READ
	add		a, c
	jr		nz, _load_bin_exit_err

	ex		af, af
	exx
	inc		a
	call	_chk_progbar
	exx
	ex		af, af

	jr		_load_bin_loop2

_load_bin_end:
	call	CMT_READ

_load_bin_exit:
#ifndef LOAD_FROM_SD
	call	CMT_CLOSE
#endif

#if 0
	ld		a, #0x21
	out		(0x30), a	// Moter Off
#endif
	ld		l, #1
	ret

_load_bin_exit_err:
#ifndef LOAD_FROM_SD
	call	CMT_CLOSE
#endif

#if 0
	ld		a, #0x21
	out		(0x30), a	// Moter Off
#endif
	ld		l, #0
	ret
__endasm;
}
