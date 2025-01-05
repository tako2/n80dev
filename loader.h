#ifndef _LOADER_H
#define _LOADER_H

#include <stddef.h>
#include <stdint.h>

extern void draw_scr(uint8_t *data) __z88dk_fastcall;
extern void chk_progbar();
extern void load_bin(const uint8_t *table) __z88dk_fastcall;

#endif // !_LOADER_H
