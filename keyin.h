#ifndef _KEYIN_H
#define _KEYIN_H

#include <stddef.h>
#include <stdint.h>

#define KEY_RET 0x80
#define KEY_6 0x40
#define KEY_SPACE 0x20
#define KEY_4 0x10
#define KEY_Z 0x08
#define KEY_2 0x04
#define KEY_X 0x02
#define KEY_8 0x01

extern uint8_t in_tenkey() __preserves_regs(b, c, iyh, iyl);

#endif
