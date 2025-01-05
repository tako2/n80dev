#ifndef _SDEX_H
#define _SDEX_H

#include <stddef.h>
#include <stdint.h>

extern uint8_t sdex_enable();

extern void sdex_send1byte(uint8_t data) __z88dk_fastcall __preserves_regs(b, c, d, e, iyh, iyl);
extern uint8_t sdex_recv1byte() __preserves_regs(b, c, d, e, iyh, iyl);
extern uint8_t sdex_exec(uint8_t cmd) __z88dk_fastcall;
extern uint8_t sdex_send_fname(const uint8_t *fname) __z88dk_fastcall __preserves_regs(c, d, e, iyh, iyl);

extern uint8_t sdex_ropen(const uint8_t *fname) __z88dk_fastcall;
extern uint8_t sdex_wnopen(const uint8_t *fname) __z88dk_fastcall;
extern uint8_t sdex_wclose();
extern uint8_t sdex_getch();
extern uint8_t sdex_putch(uint8_t ch) __z88dk_fastcall;

#endif //!_SDEX_H
