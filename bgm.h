#ifndef _BGM_H
#define _BGM_H

#include "n80dev.h"

//=============================================================================
#define BGM_BASE_CLK 20
#define BGM_WAIT_CLK(clk) (clk/BGM_BASE_CLK)

//=============================================================================
extern void bgm_start(const uint8_t *bgm, uint8_t notes);
extern void bgm_stop();
#ifndef BGM_USE_PRIORITY
extern void bgm_insert(const uint8_t *bgm, uint8_t notes);
#else
extern void bgm_insert(const uint8_t *bgm, uint8_t notes, uint8_t pri);
#endif
extern uint8_t bgm_update();
extern void bgm_set_note(uint8_t note_idx) __z88dk_fastcall;
extern void bgm_hook(uint8_t clock) __z88dk_fastcall;
extern void bgm_play_remain();
extern void bgm_swap_screen();
extern void bgm_short();
extern void bgm_long();

#endif // !_BGM_H
