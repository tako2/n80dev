#include "dirtbl.h"

#if 0  // sample
make_speed_table(normal, 0x0100);
extern const int16_t speed_normal_tbl[];
#endif

//=============================================================================
uint32_t get_speed(uint8_t dir, const int16_t *table) __naked
{
	dir; table;
__asm
	ld		iy, #0
	add		iy, sp

	ld		e, 3 (iy)
	ld		d, 4 (iy)	// DE = table

	ld		a, 2 (iy)	// A = dir

	add		a, a
	ld		l, a
	ld		h, #0
	add		hl, de

	ld		c, (hl)
	inc		hl
	ld		b, (hl)

	ld		a, 2 (iy)	// A = dir

	add		a, #8
	and		a, #0x1f
	add		a, a
	ld		l, a
	ld		h, #0
	add		hl, de

	ld		e, (hl)
	inc		hl
	ld		d, (hl)

	ex		de, hl
	ld		e, c
	ld		d, b

	// HL = dx
	// DE = dy
	ret
__endasm;
}
