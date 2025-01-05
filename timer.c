#include "n80dev.h"

//=============================================================================
#define DI __asm \
    di \
__endasm
#define EI __asm \
    ei \
__endasm

uint8_t timer_600;

uint8_t tick_count;
uint8_t next_tick;
uint8_t tick_flag;

uint8_t prev_timer;

//=============================================================================
void timer_isr() __naked
{
__asm
    di

	//push	bc
    push	hl
    push	af
	//ex		af, af
	//exx

	ld		hl, #_timer_600
	ld		a, (hl)
	inc		a
	ld		(hl), a

	ld		hl, #_next_tick
	ld		l, (hl)
	sub		a, l
	jr		nz, _timer_isr_ret

	ld		a, (_tick_count)
	add		a, l

	ld		(_next_tick), a

	ld		a, #1
	ld		(_tick_flag), a

_timer_isr_ret:
    //ld  a, (0xea55)
	ld		a, #7
    out		(0xe4), a

    pop		af
    pop		hl
	//pop		bc
	//ex		af, af
	//exx

    ei
    reti
__endasm;
}

//=============================================================================
void timer_start(uint8_t count)
{
	DI;

	tick_count = count;
	next_tick = count;
	tick_flag = 0;

    *(uint16_t *)(0x8004) = (uint16_t)timer_isr;

__asm
	ld		a, #0x80
	ld		i, a
	ld		a, #7
	out		(0xe4), a
	ld		a, #5
	out		(0xe6), a
__endasm;

    EI;
}

//=============================================================================
void timer_stop()
{
    DI;

__asm
	ld		a, #4
	out		(0xe6), a
	xor		a, a
	out		(0x40), a
__endasm;

    EI;
}

//=============================================================================
uint8_t timer_get_tick()
{
	return (tick_flag);
}

//=============================================================================
void timer_clear_tick()
{
	DI;
	tick_flag = 0;
	EI;
}

//=============================================================================
void timer_lap_start()
{
	prev_timer = timer_600;
}

//=============================================================================
uint8_t timer_lap_end()
{
	return (timer_600 - prev_timer);
}
