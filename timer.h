#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>

extern void timer_start(uint8_t tick_count);
extern void timer_stop();

extern uint8_t timer_get_tick();
extern void timer_clear_tick();

extern void timer_lap_start();
extern uint8_t timer_lap_end();

extern uint8_t timer_600;

#endif
