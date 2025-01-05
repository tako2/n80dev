#include "n80dev.h"

#define EXT_ROPEN 0x600f
#define EXT_RBYTE 0x6006
#define EXT_WAOPEN 0x6012
#define EXT_WNOPEN 0x601b
#define EXT_WBYTE 0x6018
#define EXT_WCLOSE 0x601e

#define NENTRY_VER 0x00

//=============================================================================
uint8_t load_nentry(const uint8_t *path) __z88dk_fastcall
{
	if (!sdex_enable()) return (0);

	if (sdex_ropen(path) != 0) return (0);

	if (sdex_getch() != NENTRY_VER) return (0);

	uint8_t *dest, size;
	dest = (uint8_t *)nentry_list;
	size = sizeof(NameEntry_t) * NUM_NENTRY;
	for (; size != 0; size --) {
		*dest ++ = sdex_getch();
	}

	return (1);
}

//=============================================================================
uint8_t save_nentry(const uint8_t *path) __z88dk_fastcall
{
	if (!sdex_enable()) return (0);

	if (sdex_wnopen(path) != 0) return (0);

	sdex_putch(NENTRY_VER);

	uint8_t *src, size;
	src = (uint8_t *)nentry_list;
	size = sizeof(NameEntry_t) * NUM_NENTRY;
	for (; size != 0; size --) {
		sdex_putch(*src ++);
	}

	sdex_wclose();

	return (1);

#if 0
__asm
	push	hl
	call	_sdex_enable
	ld		a, l
	pop		hl
	or		a, a
	jr		z, _save_nentry_failed

	call	_sdex_wnopen
	ld		a, l
	or		a, a
	jr		nz, _save_nentry_failed

	ld		l, #NENTRY_VER
	call	_sdex_putch

	ld		hl, #_nentry_list
	ld		b, #(8 * NUM_NENTRY)	// sizeof(NameEntry_t)
_save_nentry_loop:
	ld		a, (hl)
	inc		hl
	push	hl
	push	bc
	ld		l, a
	call	_sdex_putch
	pop		bc
	pop		hl
	djnz	_save_nentry_loop
	ld		l, #1
	ret

_save_nentry_failed:
	ld		l, #0
	ret
__endasm;
#endif
}
