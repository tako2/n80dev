#ifndef _BCD_UTILS_H
#define _BCD_UTILS_H

#include <stddef.h>
#include <stdint.h>

extern uint8_t bcd_add8(uint8_t *dest, uint8_t keta, uint8_t val) __preserves_regs(d, e, iyh, iyl);
extern uint8_t bcd_add16(uint8_t *dest, uint8_t keta, uint16_t val) __preserves_regs(d, e);

extern void bcd_to_ascii(uint8_t *bcd, uint8_t keta, uint8_t *dest);
extern void bcd_to_ascii_reg();

//extern int8_t bcd_compare(uint8_t *val1, uint8_t *val2, uint8_t keta);

extern int8_t bcd24_compare(uint8_t *bcd1, uint8_t *bcd2);

#endif
