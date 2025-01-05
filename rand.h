#ifndef __RAND_H
#define __RAND_H

#include <stddef.h>
#include <stdint.h>

extern void srand(uint16_t seed) __z88dk_fastcall;

extern uint8_t rand() __preserves_regs(b, c, d, e, iyh, iyl);

extern uint8_t hit_rand(uint8_t thresh) __z88dk_fastcall __preserves_regs(c, d, e, iyh, iyl);

#endif
