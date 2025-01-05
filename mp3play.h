#include "n80dev.h"

#ifndef _MP3_PLAY_H
#define _MP3_PLAY_H

// Media
#define DFP_USB			1		// USB
#define DFP_SD			2		// SD

extern uint8_t mp3_send_cmd(uint8_t cmd, uint8_t param1, uint8_t param2);

extern void mp3_reset();
extern void mp3_media(uint8_t media);
extern void mp3_pgm(uint8_t folder, uint8_t track);

extern void mp3_play();
extern void mp3_pause();
extern void mp3_repaet();
extern void mp3_stop();

extern void mp3_advert(uint16_t track);

#endif // !_MP3_PLAY_H
