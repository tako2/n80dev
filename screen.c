#include "n80dev.h"

//=============================================================================
#define VRAM0_ADDR 0xe700
#define VRAM1_ADDR 0xf300
#define VRAM0_ADDR_H (VRAM0_ADDR >> 8)
#define VRAM1_ADDR_H (VRAM1_ADDR >> 8)
#define VRAM_XOR (VRAM0_ADDR_H ^ VRAM1_ADDR_H)
#define VRAM0 (uint8_t *)VRAM0_ADDR
#define VRAM1 (uint8_t *)VRAM1_ADDR

//=============================================================================
uint8_t *OFFSCR_ADDR;

#ifdef DEBUG_SWAP_ASYNC
uint8_t swap_async;
#endif

extern uint8_t swap_screen_flag;
extern uint8_t prev_vrtc;

//=============================================================================
uint8_t *get_offscr_addr(uint16_t xy) __z88dk_fastcall __preserves_regs(b, c, iyh, iyl) __naked
{
    xy;
#if (ATTRS_PER_LINE == 20)
__asm
	// -7654321 0-------
    // -----765 43210---
	// bit 7,6,5 = 0
	// ----4321 0-------
	// -------- 43210---
	// use A, DE
	// total = 103clk (125clk)
	// H = x, L = y
	ld		a, (_OFFSCR_ADDR+1)	// (_OFFSCR_ADDR) is 0xf300 or 0xe700
	ld		d, a

	ld		a, l
	rl		h
	srl		l
	rr		h

	add		a, a
	add		a, a
	add		a, a
	ld		e, a
	ld		a, h
	sub		a, e
	ld		e, a

	sbc		a, a
	add		a, l
	add		a, d
	ld		d, a

	ex		de, hl
	ret
__endasm;
    //return ((uint8_t *)(OFFSCR_ADDR + (120 * (xy & 0xff)) + (xy >> 8)));
#elif (ATTRS_PER_LINE == 1)
	// total = 136clk (156clk)
	// x82 = x64 + x16 + x2 = 2 (32 + 8 + 1)
	ld		a, l
	add		a, a
	add		a, a
	add		a, a
	ld		e, a	// E = y * 8
	add		a, l
	ld		l, a	// L = y * 9

	ex		de, hl
	ld		h, #0
	add		hl, hl
	add		hl, hl	// HL = y * 32
	ld		a, d	// A = x
	ld		d, #0
	add		hl, de	// HL = y * 41
	add		hl, hl	// HL = y * 82

	ld		e, a
	ld		a, (_OFFSCR_ADDR+1)
	ld		d, a
	add		hl, de	// HL = y * 82 + x + (_OFFSCRR_ADDR)
	ret
#else // if (ATTRS_PER_LINE == 8)
__asm
	// total = 94clk (113clk)
	ld		a, (_OFFSCR_ADDR+1)
	ld		d, a

	ld		a, l
	add		a, a
	add		a, l	// A = y * 3

					// A = 76543210
	rrca
	rrca
	rrca			// A = 21076543
	ld		l, a
	and		a, #0x1f
	ld		e, a	// E = ---76543
	xor		a, l	// A = 210-----
	add		a, h	// A = A + x
	ld		l, a
	adc		a, e
	sub		a, l
	add		a, d
	ld		h, a

	ret
__endasm;
    // return ((uint8_t *)(OFFSCR_ADDR + (96 * (xy & 0xff)) + (xy >> 8)));
#endif
}

//=============================================================================
void wait_update_flag() __preserves_regs(h, l, d, e, b, c, iyh, iyl) __naked
{
__asm
_wait_update_flag_loop1:
	in		a, (0x68)
	and		a, #0x10
	jr		nz, _wait_update_flag_loop1
_wait_update_flag_loop2:
	in		a, (0x68)
	and		a, #0x10
	jr		z, _wait_update_flag_loop2
	ret
__endasm;
}

//=============================================================================
void swap_screen() __naked
{
__asm
	call	_wait_update_flag

_swap_screen_exec::	// static void swap_screen_exec()
	xor		a, a
	out		(0x66), a
	ld		a, (_OFFSCR_ADDR+1)
	out		(0x66), a
	xor		a, #VRAM_XOR
	ld		(_OFFSCR_ADDR+1), a

#ifdef COUNT_VRTC_IN_LIB
	jp		_reset_vrtc_count
#else
	ret
#endif
__endasm;
}

//=============================================================================
#ifdef COUNT_VRTC_IN_LIB
static void swap_screen_async_68h() __naked
{
__asm
	// HL = _swap_screen_flag

	in		a, (0x68)
	and		a, #0x10

#ifndef DEBUG_SWAP_ASYNC
	jp		z, _count_vrtc
#else
	jr		nz, _swap_screen_async_skip1

	ld		a, (_prev_vrtc)
	or		a, a
	jr		z, _swap_screen_async_skip2
	ld		(_swap_async), a
_swap_screen_async_skip2:

	jp		_count_vrtc

_swap_screen_async_skip1:
#endif

	// update_flag is '1'
	ld		(hl), #0		// (_swap_screen_flag) = 0
	jp		_swap_screen_exec
__endasm;
}
#endif

//=============================================================================
#ifndef RECORD_FPS
void swap_screen_async() __preserves_regs(d, e, b, c, iyh, iyl) __naked
#else
void swap_screen_async() __preserves_regs(b, c, iyh, iyl) __naked
#endif
{
__asm
#ifdef COUNT_VRTC_IN_LIB
	ld		hl, #_swap_screen_flag
	ld		a, (hl)
	dec		a
_swap_screen_async_emu:
	jr		z, _swap_screen_async_68h

	ex		af, af		// A' = swap_screen_flag - 1

	// count VRTC
	in		a, (0x40)
	and		a, #0x20

	inc		hl			// HL = _prev_vrtc
	xor		a, (hl)
	ret		z

	xor		a, (hl)
	ld		(hl), a		// store current vrtc to (_prev_vrtc)

	inc		hl
	inc		(hl)		// inc (_num_vrtc)

	ex		af, af		// A = swap_screen_flag - 1

	inc		a
	ret		z			// if (_swap_screen_flag) is 0, count VRTC only

	dec		a
	ld		(_swap_screen_flag), a

	jp		z, _swap_screen_exec
#else
#ifdef USE_NONEMU
	ld		hl, #_swap_screen_flag
	ld		a, (hl)
	dec		a
	ret		nz

	in		a, (0x68)
	and		a, #0x10
	ret		z

	xor		a, a
	ld		(hl), a

	inc		hl
	inc		hl

	ld		a, (hl)		// A = num_vrtc
	inc		hl
	ld		(hl), a		// prev_num_vrtc = A
	dec		hl
	xor		a, a
	ld		(hl), a		// num_vrtc = 0

	jp		_swap_screen_exec
#endif
#endif

	ret
__endasm;
}

//=============================================================================
void enable_swap_screen(uint8_t wait) __z88dk_fastcall __naked
{
	wait;
__asm
#ifdef DEBUG_SWAP_ASYNC
	xor		a, a
	ld		(_swap_async), a
#endif

#ifdef COUNT_VRTC_IN_LIB
	ld		a, (_num_vrtc)	// A = num_vrtc
	sla		l				// L = wait * 2
	jr		z, _enable_swap_screen_skip1

	inc		l				// L = wait * 2 + 1
	sub		a, l
	jr		nc, _enable_swap_screen_skip3

	neg						// A = wait * 2 + 1 - _num_vrtc

	ld		(_swap_screen_flag), a
	dec		a				// A = wait * 2 - _num_vrtc
	ret		nz				// if wait*2+1-_num_vrtc >= 2 return

	dec		l				// L = wait * 2 (bit0 to 0)
	// A = 0, L = wait * 2

_enable_swap_screen_skip3:
	add		a, l

_enable_swap_screen_skip1:
	// A = _num_vrtc
	and		a, #1			// A = _num_vrtc & 1
	jr		nz, _enable_swap_screen_skip4	// if A=1, in VBlank

_enable_swap_screen_chk68h:
	in		a, (0x68)
_enable_swap_screen_for_emu:
	and		a, #0x10
	jr		z, _enable_swap_screen_skip4

	ld		a, #2

_enable_swap_screen_skip4:
	inc		a				// A = (_num_vrtc & 1) + 1
	ld		(_swap_screen_flag), a

	ret
#else
_enable_swap_screen_loop:
#ifdef USE_NONEMU
	in		a, (0x68)
	and		a, #0x10
	jr		nz, _enable_swap_screen_loop
#endif
	ld		a, #1
	ld		(_swap_screen_flag), a
#endif
__endasm;
}

//=============================================================================
void swap_screen_async_remain() __naked
{
__asm
_swap_screen_async_remain_loop:
	ld		a, (_swap_screen_flag)
	or		a, a
	ret		z

	call	_swap_screen_async

	jr		_swap_screen_async_remain_loop
__endasm;
}

//=============================================================================
void init_screen_vram(uint8_t *ptr) __z88dk_fastcall __naked
{
	ptr;
__asm
	ld		a, #25
_init_screen_vram_1:
	ld		(hl), #0
	ld		e, l
	ld		d, h
	inc		de
	ld		bc, #80
	ldir

	ex		de, hl
	ld		(hl), #NO_ATTR // 0xe8
	inc		hl
	ld		(hl), #80
	ld		e, l
	ld		d, h
	inc		de
	dec		hl
	ld		bc, #(ATTRS_PER_LINE * 2 - 3)
	ldir
	ex		de, hl

	dec		a
	jr		nz, _init_screen_vram_1
	ret
__endasm;

#if 0
	uint8_t x, y;
	for (y = 25; y > 0; y --) {
		for (x = 80; x > 0; x --) {
			*ptr ++ = 0;
		}
		*ptr ++ = 0;
		*ptr ++ = NO_ATTR; // 0xe8
		for (x = 19; x > 0; x --) {
			*ptr ++ = 80;
			*ptr ++ = NO_ATTR; // 0xe8
		}
	}
#endif
}

#ifdef USE_NONEMU
//=============================================================================
const uint8_t not_for_emulator[] = "Not for emulator";
void halt_emu() __naked
{
__asm
	ld		de, #0xf300
	ld		hl, #_not_for_emulator
	ld		bc, #16
	ldir
	ld		a, #0xe8
	ld		(0xf300+81), a
	di
	halt
__endasm;
}
#else
//=============================================================================
void hook_for_emu() __naked
{
__asm
	ld		hl, #_swap_screen+1
	ld		(hl), #<_waitVBlank
	inc		hl
	ld		(hl), #>_waitVBlank

	// replace IO port 0x66 to 0x64
	ld		hl, #_swap_screen_exec+2
	dec		(hl)
	dec		(hl)
	ld		hl, #_swap_screen_exec+7
	dec		(hl)
	dec		(hl)

#ifdef COUNT_VRTC_IN_LIB
	ld		hl, #_swap_screen_async_emu+1
	ld		(hl), #0

	ld		hl, #_enable_swap_screen_for_emu+1
	ld		(hl), #0
#endif

	ret
__endasm;
}
#endif

//=============================================================================
uint8_t check_emu() __naked
{
__asm
	ld		b, #2

	in		a, (0x68)
	and		a, #0x10
	ld		c, a

__check_emu_0:
	in		a, (0x68)
	and		a, #0x10
	xor		a, c
	jr		nz, __check_emu_2

	in		a, (0x40)
	and		a, #0x20
	jr		nz, __check_emu_0

__check_emu_1:
	in		a, (0x68)
	and		a, #0x10
	xor		a, c
	jr		nz, __check_emu_2

	in		a, (0x40)
	and		a, #0x20
	jr		z, __check_emu_1

	djnz	__check_emu_0

#ifndef USE_NONEMU
	call	_hook_for_emu
#endif

	ld		l, #1
	ret

__check_emu_2:
	ld		l, #0
	ret
__endasm;
}

//=============================================================================
uint8_t init_screen()
{
	init_crtc();

	init_screen_vram(VRAM0);
	init_screen_vram(VRAM1);

	OFFSCR_ADDR = VRAM1;

	// check emu or non-emu
	return (check_emu());
}

//=============================================================================
void _fill_line() __naked
{
__asm
	// HL: start addr
	// BC: fill pattern
	// DE: work
	// IY: to store sp
	di
	ld		de, #80
	add		hl, de
	ld		iy, #0
	add		iy, sp
	ld		sp, hl

	.rept	40
	push	bc
	.endm

	ld		sp, iy
	ei
	ret
__endasm;
}

//=============================================================================
void clear_vram(uint8_t start_line, uint8_t num_lines) __naked
{
    start_line; num_lines;
__asm
	ld		hl, #3
	add		hl, sp

	ld		b, (hl) 			// B = num_lines
	dec		hl
	ld		l, (hl)
	ld		h, #0
    call    _get_offscr_addr	// HL = VRAM addr

	ld		a, b
	ld		bc, #0

_clear_vram_0:
	call	__fill_line

	// skip attributes area, (clear last byte as used N of attrs)
#if (ATTRS_PER_LINE == 20)
	ld		de, #40-1
	add		hl, de
	ld		(hl), #0
	inc		hl
#else
	ld		de, #(ATTRS_PER_LINE * 2)
	add		hl, de
#endif

	dec		a
	jr		nz, _clear_vram_0

#ifdef COUNT_VRTC_IN_LIB
	call	_count_vrtc
#endif

	ret
__endasm;
}

//=============================================================================
void clear_screen(uint8_t start_line, uint8_t num_lines) __naked
{
	start_line; num_lines;
__asm
	jp		_clear_vram
__endasm;

#if 0
	clear_vram(start_line, num_lines);
#endif
}
