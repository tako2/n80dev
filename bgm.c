#include "n80dev.h"

//=============================================================================
#define BGM_PLAY_REM 16
#define BGM_PLAY_NOTE(clk) { (((clk/2)/BGM_BASE_CLK) - BGM_PLAY_REM), ((60000/clk) | 1) }

typedef struct {
	uint8_t clock;
	uint8_t loop;
} PlayNote_t;

const PlayNote_t bgm_play_note_table[] = {
	// rest
	{ 0, 0 },

	// o2
	BGM_PLAY_NOTE(10204),// g
	BGM_PLAY_NOTE(9632), // g+
	BGM_PLAY_NOTE(9090), // a
	BGM_PLAY_NOTE(8580), // a+
	BGM_PLAY_NOTE(8100), // b

	// o3
	BGM_PLAY_NOTE(7645), // c
	BGM_PLAY_NOTE(7215), // c+
	BGM_PLAY_NOTE(6810), // d
	BGM_PLAY_NOTE(6428), // d+
	BGM_PLAY_NOTE(6068), // e
	BGM_PLAY_NOTE(5727), // f
	BGM_PLAY_NOTE(5405), // f+
	BGM_PLAY_NOTE(5102), // g
	BGM_PLAY_NOTE(4816), // g+
	BGM_PLAY_NOTE(4545), // a
	BGM_PLAY_NOTE(4290), // a+
	BGM_PLAY_NOTE(4050), // b

	// o4
	BGM_PLAY_NOTE(3823), // c
	BGM_PLAY_NOTE(3608), // c+
	BGM_PLAY_NOTE(3406), // d
	BGM_PLAY_NOTE(3214), // d+
	BGM_PLAY_NOTE(3034), // e
	BGM_PLAY_NOTE(2864), // f
	BGM_PLAY_NOTE(2703), // f+
	BGM_PLAY_NOTE(2551), // g
	BGM_PLAY_NOTE(2408), // g+
	BGM_PLAY_NOTE(2273), // a
	BGM_PLAY_NOTE(2145), // a+
	BGM_PLAY_NOTE(2025), // b

	// o5
	BGM_PLAY_NOTE(1911), // c
	BGM_PLAY_NOTE(1804), // c+
	BGM_PLAY_NOTE(1703), // d
	BGM_PLAY_NOTE(1607), // d+
	BGM_PLAY_NOTE(1517), // e
};

//=============================================================================
uint8_t *bgm_notes;
uint8_t max_notes;

uint8_t next_note;

const uint8_t *bgm2_notes;
uint8_t bgm2_remain;
#ifdef BGM_USE_PRIORITY
uint8_t bgm2_pri;
#endif

//=============================================================================
void bgm_start(const uint8_t *bgm, uint8_t notes) __naked
{
	bgm; notes;
__asm
	ld		hl, #2
	add		hl, sp
	ld		e, (hl)
	inc		hl
	ld		d, (hl)
	inc		hl
	ld		a, (hl)

	ld		(_max_notes), a
	ex		de, hl
	ld		(_bgm_notes), hl

	xor		a, a
	ld		(_next_note), a

	ld		(_bgm2_remain), a
#ifdef BGM_USE_PRIORITY
	ld		(_bgm2_pri), a
#endif

	ret
__endasm;
#if 0
	bgm_notes = bgm;
	max_notes = notes;

	next_note = 0;

	bgm2_remain = 0;
#endif
}

//=============================================================================
void bgm_stop()
{
__asm
	exx
	ld		b, #0
	exx
	xor		a, a
	out		(0x40), a
__endasm;

	bgm_notes = NULL;

	bgm2_remain = 0;
#ifdef BGM_USE_PRIORITY
	bgm2_pri = 0;
#endif
}

//=============================================================================
#ifndef BGM_USE_PRIORITY
void bgm_insert(const uint8_t *bgm, uint8_t notes) __naked
#else
void bgm_insert(const uint8_t *bgm, uint8_t notes, uint8_t pri) __naked
#endif
{
	bgm; notes;
#ifdef BGM_USE_PRIORITY
	pri;
#endif
__asm
	ld		hl, #2
	add		hl, sp

	ld		e, (hl)
	inc		hl
	ld		d, (hl)		// DE = bgm
	inc		hl
#ifndef BGM_USE_PRIORITY
	ld		a, (hl)		// A = ntoes
#else
	ld		c, (hl)		// C = notes
	inc		hl
	ld		a, (_bgm2_pri)
	ld		b, a
	ld		a, (hl)		// A = pri
	cp		b
	ret		c
	ld		(_bgm2_pri), a
	ld		a, c		// A = notes
#endif

	ld		(_bgm2_remain), a
	ex		de, hl
	ld		(_bgm2_notes), hl

	ret
__endasm;
#if 0
	bgm2_notes = bgm;
	bgm2_remain = notes;
#endif
}

//=============================================================================
uint8_t bgm_update() __naked
{
__asm
	ld		a, (_bgm2_remain)
	or		a, a
	jr		z, _bgm_update_skip1

	dec		a
	ld		(_bgm2_remain), a
#ifdef BGM_USE_PRIORITY
	jr		nz, _bgm_update_skip_p1
	ld		(_bgm2_pri), a
_bgm_update_skip_p1:
#endif

	ld		hl, (_bgm2_notes)
	ld		a, (hl)
	inc		hl
	ld		(_bgm2_notes), hl
	ld		l, a

	// skip main bgm
	ld		a, (_max_notes)
	ld		h, a
	ld		a, (_next_note)
	inc		a
	cp		h
	jr		nz, _bgm_update_skip3
	xor		a, a
_bgm_update_skip3:
	ld		(_next_note), a

	ret

_bgm_update_skip1:
#ifdef BGM_MUTE
	in		a, (8)
	and		a, #0x20
	ld		l, a
	ret		z
#endif

	ld		hl, (_bgm_notes)
	ld		a, l
	or		a, h
	ret		z

	ld		a, (_next_note)
	ld		e, a
	ld		d, #0
	add		hl, de

	ld		d, (hl)
	inc		e
	ld		a, (_max_notes)
	cp		e
	jr		z, _bgm_update_skip2

	ld		a, e
	ld		(_next_note), a
	ld		l, d
	ret

_bgm_update_skip2:
	xor		a, a
	ld		(_next_note), a
	ld		l, d
	ret
__endasm;

#if 0
	uint8_t idx;

	if (bgm2_remain != 0) {
		bgm2_remain --;

		idx = *bgm2_notes;
		bgm2_notes ++;

		return (idx);
	}

#ifdef BGM_MUTE
__asm
	in		a, (8)
	and		a, #0x20
	ld		l, a
	ret		z
__endasm;
#endif

	if (bgm_notes == NULL) return (0);

	idx = bgm_notes[next_note];
	next_note ++;
	if (next_note >= max_notes) {
		next_note = 0;
	}

	return (idx);
#endif
}

//=============================================================================
void bgm_set_note(uint8_t note_idx) __z88dk_fastcall __naked
{
	note_idx;
__asm
	// output
	// H' = clock_base
	// L' = clock_work
	// B' = loop
	// C' = beep_on

	ld		a, l
	add		a, a
	add		a, #<_bgm_play_note_table
	ld		l, a
	adc		a, #>_bgm_play_note_table
	sub		a, l
	ld		h, a

	ld		a, (hl)		// A = clock
	inc		hl
	ld		b, (hl)		// B = loop

	ld		l, a
	ld		h, a		// H = L = A (clock)

	ld		a, b
	and		a, a
	jr		z, _bgm_set_note_ret

	ld		c, #0x20
	ld		a, c
	out		(0x40), a

_bgm_set_note_ret:
	exx
	ret
__endasm;
}

//=============================================================================
void bgm_hook_exec() __naked
{
__asm
	ld		a, l
	add		a, #BGM_PLAY_REM
_bgm_hook_exec_loop:
	dec		a
	jr		nz, _bgm_hook_exec_loop

	// bgm_play_note.loop --;
	dec		b
	jr		z, _bgm_hook_exec_last

	ld		a, c
	xor		a, #0x20
	ld		c, a
	out		(0x40), a

	ld		l, h
	ret

_bgm_hook_exec_last:
	xor		a, a
	out		(0x40), a
	ret
__endasm;
}

//=============================================================================
void bgm_hook(uint8_t clock) __z88dk_fastcall __naked
{
	clock;
__asm
	ld		a, l

	exx
	ld		e, a
	ld		a, b		// B = loop count
	and		a, a
	jr		z, _bgm_hook_ret

	ld		a, l
	sub		a, e
	ld		l, a
	jr		nc, _bgm_hook_ret

	add		a, #BGM_PLAY_REM
_bgm_hook_loop:
	dec		a
	jr		nz, _bgm_hook_loop

	dec		b
	jr		z, _bgm_hook_last

	ld		a, c
	xor		a, #0x20
	ld		c, a
	out		(0x40), a

	ld		l, h
	exx
	ret

_bgm_hook_last:
	xor		a, a
	out		(0x40), a

_bgm_hook_ret:
	exx
	ret
__endasm;
}

//=============================================================================
void bgm_play_remain() __naked
{
	// H' = clock_base
	// L' = clock_work
	// B' = loop
	// C' = beep_on
__asm
	exx
	ld		a, b
	and		a, a
	ret		z

_bgm_play_remain_loop:
	ld		a, l
	add		a, #BGM_PLAY_REM

_bgm_play_remain_loop0:
	dec		a
	jr		nz, _bgm_play_remain_loop0

	// bgm_play_note.loop --;
	dec		b
	jr		z, _bgm_play_remain_last

	ld		a, c
	xor		a, #0x20
	ld		c, a
	out		(0x40), a

	ld		l, h

#ifdef COUNT_VRTC_IN_LIB
	exx
	call	_count_vrtc
	//call	_swap_screen_async
	exx
#endif

	jr		_bgm_play_remain_loop

_bgm_play_remain_last:
	xor		a, a
	out		(0x40), a

	ret
__endasm;
}

//=============================================================================
void bgm_short() __naked
{
__asm
	exx
	ld		a, h
	and		a, a	// CY = 0
	jr		z, _bgm_short_ret

	ld		a, b
	rra
#if 0
	ld		b, a
	and		a, a
	rra
	add		a, b
#endif
	or		a, #1
	ld		b, a

_bgm_short_ret:
	exx
	ret
__endasm;
}

//=============================================================================
void bgm_long() __naked
{
__asm
	exx
	ld		a, h
	and		a, a	// CY = 0
	jr		z, _bgm_long_ret

	ld		a, b
	rlca
	or		a, #1
	ld		b, a

_bgm_long_ret:
	exx
	ret
__endasm;
}
