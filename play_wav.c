#include "n80dev.h"

#define USE_WAIT

void play_wav(const uint8_t *data, uint16_t size) __naked
{
__asm
	//xor		a, a
	//out		(0x51), a

	pop		bc
	pop		hl		// HL = data
	pop		de		// DE = size
	push	de
	push	hl
	push	bc

_play_loop2:
	ld		c, (hl)
	inc		hl
	ld		b, #8

_play_loop1:
	rlc		c
	jr		c, _play_beep_on

	xor		a, a
	out		(0x40), a

#ifdef USE_WAIT
	call	_wait
#endif

	jr		_play_skip1

_play_beep_on:
	ld		a, #0x20
	out		(0x40), a

#ifdef USE_WAIT
	call	_wait
#endif

_play_skip1:
	djnz	_play_loop1

	dec		de
	ld		a, e
	or		a, d
	jr		nz, _play_loop2

	xor		a, a
	out		(0x40), a

	ret

#ifdef USE_WAIT
_wait:
	push	bc
	in		a, (0x40)
	and		a, #0x20
	jr		nz, _wait_skip1
	ld		b, #52
	jr		_wait_loop
_wait_skip1:
	ld		b, #120
_wait_loop:
	djnz	_wait_loop
	pop		bc
	ret
#endif
__endasm;
}
