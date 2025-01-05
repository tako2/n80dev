#ifndef _ROTATE_H
#define _ROTATE_H

#include <stddef.h>
#include <stdint.h>

typedef union {
	uint32_t val;
	struct {
		int16_t x;
		int16_t y;
	};
} Vector_t;

extern uint32_t rotate_small(int16_t x, int16_t y);
extern uint32_t rotate32(int16_t x, int16_t y, uint8_t dir);

#endif // !_ROTATE_H
