#ifndef _GET_DIR_H
#define _GET_DIR_H

#include <stddef.h>
#include <stdint.h>

extern uint8_t get_dir32(int8_t dx, int8_t dy);
extern uint8_t get_dir32_stub(uint16_t dy_dx) __z88dk_fastcall;

#endif // !_GET_DIR_H
