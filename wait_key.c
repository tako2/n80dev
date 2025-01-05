#include "n80dev.h"

#define WAIT_KEY (KEY_SPACE|KEY_X|KEY_Z)

//=============================================================================
void wait_loop(uint8_t frame) __z88dk_fastcall
{
	for (; frame != 0; frame --) {
		waitVBlank();
	}
}

//=============================================================================
uint8_t check_space_key() __naked
{
__asm
	call	_in_tenkey
	ld		a, l
	and		a, #WAIT_KEY
	cp		#WAIT_KEY
	jr		z, _check_space_key_skip1
	ld		l, #1
	ret
_check_space_key_skip1:
	ld		l, #0
	ret
__endasm;
}

//=============================================================================
uint8_t wait_space_key(uint8_t frames) __z88dk_fastcall __naked
{
	frames;
__asm
	ld		b, l
_wait_space_key_loop:
#if 0
	call	_in_tenkey
	ld		a, l
	and		a, #WAIT_KEY
	cp		#WAIT_KEY
	jr		z, _wait_space_key_skip1
	ld		l, #1
	ret
_wait_space_key_skip1:
#else
	call	_check_space_key
	ret		nz
#endif
	call	_waitVBlank
	djnz	_wait_space_key_loop
	ld		l, #0
	ret
__endasm;
}
