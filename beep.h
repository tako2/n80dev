#ifndef _BEEP_H
#define _BEEP_H

#include <stddef.h>
#include <stdint.h>

enum {
	NOTE_NONE = 0,

	NOTE_O2_G,
	NOTE_O2_GS,
	NOTE_O2_A,
	NOTE_O2_AS,
	NOTE_O2_B,

	NOTE_O3_C,
	NOTE_O3_CS,
	NOTE_O3_D,
	NOTE_O3_DS,
	NOTE_O3_E,
	NOTE_O3_F,
	NOTE_O3_FS,
	NOTE_O3_G,
	NOTE_O3_GS,
	NOTE_O3_A,
	NOTE_O3_AS,
	NOTE_O3_B,

	NOTE_O4_C,
	NOTE_O4_CS,
	NOTE_O4_D,
	NOTE_O4_DS,
	NOTE_O4_E,
	NOTE_O4_F,
	NOTE_O4_FS,
	NOTE_O4_G,
	NOTE_O4_GS,
	NOTE_O4_A,
	NOTE_O4_AS,
	NOTE_O4_B,

	NOTE_O5_C,
	NOTE_O5_CS,
	NOTE_O5_D,
	NOTE_O5_DS,
	NOTE_O5_E,

	NOTE_LOOP = 128
};

extern void bgm_play(uint8_t *bgm, uint8_t notes, uint8_t tempo);

extern void beep(uint8_t on, uint8_t off, uint8_t cycle);
extern void beep_note(uint8_t note_idx);

#endif
