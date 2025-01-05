#include "n80dev.h"

// for PC-8001mkII
#define PPI_A 0x7c
#define PPI_B 0x7d
#define PPI_C 0x7e
#define PPI_R 0x7f

#define EXT_ROPEN 0x600f
#define EXT_RBYTE 0x6006
#define EXT_WAOPEN 0x6012
#define EXT_WNOPEN 0x601b
#define EXT_WBYTE 0x6018
#define EXT_WCLOSE 0x601e

//=============================================================================
uint8_t sdex_enable() __naked
{
__asm
	ld		hl, #0x6000
	ld		a, (hl)
	cp		#'A'
	jr		nz, _sdex_enable_failed
	inc		hl
	ld		a, (hl)
	cp		#'B'
	jr		nz, _sdex_enable_failed
	inc		hl
	ld		a, (hl)
	or		a, a
	jr		nz, _sdex_enable_failed
	ld		l, #1
	ret
_sdex_enable_failed:
	ld		l, #0
	ret
__endasm;
}

//=============================================================================
static void wait_busy_up() __naked
{
__asm
	in		a, (PPI_C)
	and		a, #0x80
	jr		z, _wait_busy_up
	ret
__endasm;
}

//=============================================================================
static void wait_busy_down() __naked
{
__asm
	in		a, (PPI_C)
	and		a, #0x80
	jr		nz, _wait_busy_down
	ret
__endasm;
}

//=============================================================================
static void send4bit(/*uint8_t data*/) __naked
{
__asm
	// A: 4bit data to send
	out		(PPI_A), a

	ld		a, #0x05
	out		(PPI_R), a

	call	_wait_busy_up

	ld		a, #0x04
	out		(PPI_R), a

	call	_wait_busy_down

	ret
__endasm;
}

//=============================================================================
void sdex_send1byte(uint8_t data) __z88dk_fastcall __preserves_regs(b, c, d, e, iyh, iyl) __naked
{
	data;
__asm
	ld		a, l
_sdex_send1byte_areg:
	push	af
	rra
	rra
	rra
	rra
	and		a, #0x0f
	call	_send4bit
	pop		af
	and		a, #0x0f
	call	_send4bit
	ret
__endasm;
}

//=============================================================================
uint8_t sdex_recv1byte() __preserves_regs(b, c, d, e, iyh, iyl) __naked
{
__asm
	call	_wait_busy_up

	in		a, (PPI_B)
	ld		l, a

	ld		a, #0x05
	out		(PPI_R), a

	call	_wait_busy_down

	ld		a, #0x04
	out		(PPI_R), a

	ret
__endasm;
}

//=============================================================================
uint8_t sdex_exec(uint8_t cmd) __z88dk_fastcall
{
	cmd;
	sdex_send1byte(cmd);
	return (sdex_recv1byte()); // 0x00 is SUCCESS
}

//=============================================================================
uint8_t sdex_send_fname(const uint8_t *fname) __z88dk_fastcall __preserves_regs(c, d, e, iyh, iyl) __naked
{
	fname;
__asm
	ld		b, #32

_sdex_send_fname_loop:
	ld		a, (hl)
	inc		hl
	call	_sdex_send1byte_areg
	djnz	_sdex_send_fname_loop

	ld		a, #0x0d
	call	_sdex_send1byte_areg

	jp		_sdex_recv1byte
__endasm;
}

//=============================================================================
uint8_t sdex_ropen(const uint8_t *fname) __z88dk_fastcall
{
	uint8_t err;
	err = sdex_exec(0x76);
	if (err != 0) return (err);

	return (sdex_send_fname(fname));
}

//=============================================================================
uint8_t sdex_wnopen(const uint8_t *fname) __z88dk_fastcall
{
	uint8_t err;
	err = sdex_exec(0x79);
	if (err != 0) return (err);

	return (sdex_send_fname(fname));
}

//=============================================================================
uint8_t sdex_waopen(const uint8_t *fname) __z88dk_fastcall
{
	uint8_t err;
	err = sdex_exec(0x77);
	if (err != 0) return (err);

	return (sdex_send_fname(fname));
}

//=============================================================================
uint8_t sdex_wclose()
{
	return (sdex_exec(0x7b));
}

//=============================================================================
uint8_t sdex_getch()
{
	sdex_exec(0x72);
	return (sdex_recv1byte());
}

//=============================================================================
uint8_t sdex_putch(uint8_t ch) __z88dk_fastcall
{
	uint8_t err;
	err = sdex_exec(0x78);
	if (err != 0) return (err);

	sdex_send1byte(ch);
	return (sdex_recv1byte()); // 0x00 is SUCCESS
}
