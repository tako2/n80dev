#include "n80dev.h"

#ifdef BGM_USE_NOTE
typedef struct {
	uint8_t onoff;
	uint8_t cycle;
} Note_t;

#ifdef BUILD_N80
#define NOTE_ONOFF(clk) ((clk-(74*2)) / 64)
#else
#define NOTE_ONOFF(clk) ((clk-74) / 64)
#endif

const Note_t note_table[] = {
	// rest
	{   0,  0 },

	// o3
	{ NOTE_ONOFF(10204), 4 }, // g
	{ NOTE_ONOFF(9632),  4 }, // g+
	{ NOTE_ONOFF(9090),  4 }, // a
	{ NOTE_ONOFF(8580),  4 }, // a+
	{ NOTE_ONOFF(8100),  4 }, // b

	// o4
	{ NOTE_ONOFF(7645),  5 }, // c
	{ NOTE_ONOFF(7215),  5 }, // c+
	{ NOTE_ONOFF(6810),  5 }, // d
	{ NOTE_ONOFF(6428),  6 }, // d+
	{ NOTE_ONOFF(6068),  6 }, // e
	{ NOTE_ONOFF(5727),  6 }, // f
	{ NOTE_ONOFF(5405),  7 }, // f+
	{ NOTE_ONOFF(5102),  7 }, // g
	{ NOTE_ONOFF(4816),  8 }, // g+
	{ NOTE_ONOFF(4545),  8 }, // a
	{ NOTE_ONOFF(4290),  9 }, // a+
	{ NOTE_ONOFF(4050),  9 }, // b

	// o5
	{ NOTE_ONOFF(3823), 10 }, // c
	{ NOTE_ONOFF(3608), 11 }, // c+
	{ NOTE_ONOFF(3406), 11 }, // d
	{ NOTE_ONOFF(3214), 12 }, // d+
	{ NOTE_ONOFF(3034), 13 }, // e
	{ NOTE_ONOFF(2864), 13 }, // f
	{ NOTE_ONOFF(2703), 14 }, // f+
	{ NOTE_ONOFF(2551), 15 }, // g
	{ NOTE_ONOFF(2408), 16 }, // g+
	{ NOTE_ONOFF(2273), 17 }, // a
	{ NOTE_ONOFF(2145), 18 }, // a+
	{ NOTE_ONOFF(2025), 19 }, // b

	{ NOTE_ONOFF(1911), 20 }, // c
	{ NOTE_ONOFF(1804), 22 }, // c+
	{ NOTE_ONOFF(1703), 22 }, // d
	{ NOTE_ONOFF(1607), 24 }, // d+
	{ NOTE_ONOFF(1517), 26 }, // e
};

#define BEEP_ON __asm \
	ld	a, #0x20 \
	out	(0x40), a \
__endasm
#define BEEP_OFF __asm \
	xor	a, a \
	out	(0x40), a \
__endasm

//=============================================================================
void bgm_wait(uint16_t cycle) __z88dk_fastcall
{
	cycle;
	// wait (cycle * 100) clk
__asm
_bgm_wait_1:
	ld		a, h
	or		a, l
	ret		z

	ld		b, #5
_bgm_wait_2:
#ifndef BUILD_N80
	nop					//  4 clk
	nop					//  4 clk
#endif
	djnz	_bgm_wait_2	// 8/13 clk

	dec		hl
	jr		_bgm_wait_1
__endasm;
}

//=============================================================================
void bgm_play(uint8_t *bgm, uint8_t notes, uint8_t tempo)
{
	const Note_t *note;
	uint16_t none_cyc;
	uint16_t skip_cyc;

	if (tempo < 16) tempo = 16;

	none_cyc = tempo * 100;
	skip_cyc = none_cyc - 1600;

	for (; notes > 0; notes --) {
		note = &note_table[*bgm++];
		if (note->cycle == 0) {
			//cycle = 1158 * tempo;
			//for (; cycle > 0; cycle --) ;
			bgm_wait(none_cyc);
			continue;
		}

		// about 40,000 clk ~ 10ms
		// muptiply by 4 => 40ms
		beep(note->onoff, note->onoff, note->cycle * 4);

		//cycle = 1158 * (tempo - 4);
		//for (; cycle > 0; cycle --) ;
		bgm_wait(skip_cyc);
	}
}

//=============================================================================
void beep_note(uint8_t note_idx)
{
	const Note_t *note;

	note = &note_table[note_idx];

	beep(note->onoff, note->onoff, note->cycle);
}
#endif

//=============================================================================
void beep(uint8_t on, uint8_t off, uint8_t cycle)
{
	on; off; cycle;
__asm
	ld		iy, #0				// 14 clk
	add		iy, sp				// 15 clk

	ld		b, 4 (iy)			// 19 clk

_beep_loop_1:
	ld		a, #0x20			//  7 clk
	out		(0x40), a			// 11 clk

	ld		c, 2 (iy)			// 19 clk
_beep_loop_2:
#ifndef BUILD_N80
	nop							//  4 clk
	nop							//  4 clk
	nop							//  4 clk
	nop							//  4 clk
#endif
	dec		c					//  4 clk
	jr		nz, _beep_loop_2	// 7/12 clk
	// 19 + (N - 1) * 32 + 27 = (N * 32) + 14 clk

	xor		a, a				//  4 clk
	out		(0x40), a			// 11 clk

	ld		c, 3 (iy)			// 19 clk
_beep_loop_3:
#ifndef BUILD_N80
	nop							//  4 clk
	nop							//  4 clk
	nop							//  4 clk
	nop							//  4 clk
#endif
	dec		c					//  4 clk
	jr		nz, _beep_loop_3	// 7/12 clk

	djnz	_beep_loop_1		// 8/13 clk
	// 18 + ((N * 32) + 14) * 2 + 15 + 13 = (N * 64) + 74 (for 1 loop)
	// => reffer NOTE_ONOFF() macro
__endasm;
}
