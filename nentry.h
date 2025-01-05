#ifndef _NENTRY_H
#define _NENTRY_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint8_t score[3];	// BCD
	uint8_t name[4];
	uint8_t stage;
} NameEntry_t;

#define NUM_NENTRY 5
extern NameEntry_t nentry_list[NUM_NENTRY];

extern void init_nentry(const NameEntry_t *template) __z88dk_fastcall;
extern int8_t insert_nentry(NameEntry_t *new) __z88dk_fastcall;
extern void copy_nentry_name(uint8_t rank, uint8_t *name);
extern NameEntry_t *get_nentry(uint8_t rank) __z88dk_fastcall __preserves_regs(b, c, d, e, iyh, iyl);

extern uint8_t load_nentry(const uint8_t *path) __z88dk_fastcall;
extern uint8_t save_nentry(const uint8_t *path) __z88dk_fastcall;

#endif // !_NENTRY_H
