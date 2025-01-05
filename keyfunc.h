#ifndef _KEYFUNC_H
#define _KEYFUNC_H

#include <stddef.h>
#include <stdint.h>

#define KEY_ESC		0x80
#define KEY_SHIFT	0x40
#define KEY_F5		0x20
#define KEY_F4		0x10
#define KEY_F3		0x08
#define KEY_F2		0x04
#define KEY_F1		0x02
#define KEY_STOP	0x01

extern uint8_t in_funckey();

#endif	// !_KEYFUNC_H
