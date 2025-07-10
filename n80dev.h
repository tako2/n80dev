#ifndef _N80DEV_H
#define _N80DEV_H

#include "n80dev_config.h"

#include "keyin.h"
#include "keyfunc.h"
#include "timer.h"
#include "beep.h"
#include "bgm.h"
#include "screen.h"
#include "bitmap.h"
#include "insert_attr.h"
#include "draw_pixel.h"
#include "font.h"
#include "rand.h"
#include "pcg.h"
#include "bcd_utils.h"
#include "list.h"
#include "dirtbl.h"
#include "collision.h"
#include "nentry.h"
#include "get_dir.h"
#include "loader.h"
#include "line.h"
#include "rotate.h"
#include "mp3play.h"
#include "ranking.h"

#include "sdex.h"

#ifdef ENABLE_ASSERT
#define assert(c) if (!(c)) { \
	__asm \
	di \
	halt \
	__endasm; \
	} \
	(void)0
#else
#define assert(c) (void)0
#endif

extern void init_crtc();
extern void waitVBlank() __preserves_regs(b, c, d, e, h, l, iyh, iyl);

extern void wait_loop(uint8_t frame) __z88dk_fastcall;
extern uint8_t check_space_key();
extern uint8_t wait_space_key(uint8_t frames) __z88dk_fastcall;

#ifndef RECORD_FPS
extern void reset_vrtc_count() __preserves_regs(b, c, d, e, iyh, iyl);
#else
extern void reset_vrtc_count() __preserves_regs(b, c, iyh, iyl);
extern uint8_t fps;
extern void reset_fps();
#endif
extern void count_vrtc() __preserves_regs(b, c, d, e, iyh, iyl);
extern uint8_t num_vrtc;
extern uint8_t prev_num_vrtc;

extern void play_wav(const uint8_t *data, uint16_t size);

#endif // !_N80DEV_H

