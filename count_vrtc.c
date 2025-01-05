#include "n80dev.h"

//=============================================================================
uint8_t swap_screen_flag;
uint8_t prev_vrtc;
uint8_t num_vrtc;		// need this order, place this after 'prev_vrtc'
uint8_t prev_num_vrtc;

#ifdef RECORD_FPS
uint8_t num_frames;
uint8_t cnt_frames;
uint8_t fps;

uint8_t his_frames;
uint8_t his_frames_list[8];
uint8_t his_ptr;
#endif

//=============================================================================
#ifdef RECORD_FPS
void reset_fps() __naked
{
__asm
#if 1
	xor		a, a
	ld		hl, #_his_frames
	ld		b, #10
_reset_fps_loop:
	ld		(hl), a
	inc		hl
	djnz	_reset_fps_loop
#endif

	xor		a, a
	ld		hl, #_num_frames
	ld		(hl), a
	inc		hl
	ld		(hl), a
	inc		hl
	ld		(hl), a
	ret
__endasm;
}
#endif

//=============================================================================
#ifndef RECORD_FPS
void reset_vrtc_count() __preserves_regs(b, c, d, e, iyh, iyl) __naked
{
#if 0
	prev_vrtc = 0x20;
	num_vrtc = 0;
#endif

__asm
	ld		a, (_num_vrtc)
	ld		(_prev_num_vrtc), a
	ld		hl, #0x0000
	ld		(_prev_vrtc), hl
	ret
__endasm;
}
#else
void reset_vrtc_count() __preserves_regs(b, c, iyh, iyl) __naked
{
__asm
	ld		hl, #_cnt_frames

	// use BCD
	ld		a, (hl)
	inc		a
	daa
	ld		(hl), a			// cnt_frames ++ (as BCD)

	ex		de, hl

	ld		hl, #_num_frames
	ld		a, (_num_vrtc)
	srl		a
	add		a, (hl)

	ld		(hl), a			// num_frames += (num_vrtc / 2)
	cp		#60
	jr		c, _reset_vrtc_count_skip
	sub		a, #60
	ld		(hl), a			// num_frames -= 60

	ld		a, (de)
	ld		(_fps), a		// fps = cnt_frames
	xor		a, a
	ld		(de), a			// cnt_frames = 0

_reset_vrtc_count_skip:
#if 1
	ld		hl, #_his_ptr
	ld		a, (hl)
	inc		a
	and		a, #7
	ld		(hl), a

	add		a, #<_his_frames_list
	ld		l, a
	adc		a, #>_his_frames_list
	sub		a, l
	ld		h, a

	ld		a, (hl)
	ex		af, af
	ld		a, (_num_vrtc)
	srl		a
	
	ld		(hl), a		// his_frames_list[his_ptr] = _num_vrtc
	ld		h, a		// H = _num_vrtc
	ex		af, af
	ld		l, a		// L = his_frames_list[his_ptr]

	ld		de, #_his_frames
	ld		a, (de)
	sub		a, l
	add		a, h
	ld		(de), a
#endif

	ld		a, (_num_vrtc)
	ld		(_prev_num_vrtc), a
	ld		hl, #0x0000
	ld		(_prev_vrtc), hl
	ret
__endasm;
}
#endif

//=============================================================================
void count_vrtc() __preserves_regs(b, c, d, e, iyh, iyl) __naked
{
__asm
	in		a, (0x40)
	and		a, #0x20

	ld		hl, #_prev_vrtc
	xor		a, (hl)		// 0 ^ 1 => 1, 1 ^ 0 => 1
	ret		z
	// 40(45) clk

	xor		a, (hl)		// 1 ^ 1 => 0, 1 ^ 0 => 1
	ld		(hl), a

	inc		hl			// _num_vrtc is placed 1 byte after
	inc		(hl)

	ret
	// 81(91) clk
__endasm;
}
