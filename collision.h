#ifndef _COLLISION_H
#define _COLLISION_H

#include <stddef.h>
#include <stdint.h>

extern uint8_t detect_collision(uint8_t x1, uint8_t x2, uint8_t w1, uint8_t w2) __preserves_regs(d, e, iyh, iyl);
extern uint8_t detect_collision0(uint32_t x1w1x2w2) __z88dk_fastcall;
extern uint8_t detect_collision2(uint16_t val) __z88dk_fastcall;
extern uint8_t detect_collision4(uint16_t val) __z88dk_fastcall;

extern uint8_t detect_collision0_cy(uint32_t x1w1x2w2) __z88dk_fastcall;
extern uint8_t detect_collision2_cy(uint16_t val) __z88dk_fastcall;
extern uint8_t detect_collision4_cy(uint16_t val) __z88dk_fastcall;

#endif // !_COLLISION_H
