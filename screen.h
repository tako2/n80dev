#ifndef _SCREEN_H
#define _SCREEN_H

#include <stddef.h>
#include <stdint.h>

#define NO_ATTR 0x18

extern uint8_t *OFFSCR_ADDR;

extern uint8_t init_screen();
extern void clear_screen(uint8_t start_line, uint8_t num_lines);
extern void swap_screen();
extern uint8_t *get_offscr_addr(uint16_t xy) __z88dk_fastcall __preserves_regs(b, c, iyh, iyl);

extern void wait_update_flag() __preserves_regs(h, l, d, e, b, c, iyh, iyl);
#ifndef RECORD_FPS
extern void swap_screen_async() __preserves_regs(d, e, b, c, iyh, iyl);
#else
extern void swap_screen_async() __preserves_regs(b, c, iyh, iyl);
#endif
extern void enable_swap_screen(uint8_t wait) __z88dk_fastcall;
extern void swap_screen_async_remain();

#ifdef USE_NONEMU
extern void halt_emu();
#else
#define halt_emu() (void)(0)
#endif

#endif // !_SCREEN_H
