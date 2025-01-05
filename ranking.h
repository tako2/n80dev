#ifndef __RANKING_H
#define __RANKING_H

#include <stddef.h>
#include <stdint.h>
#include "nentry.h"
#include "keyin.h"

extern uint8_t nentry_cur;
extern NameEntry_t ins_nent;

extern int8_t insert_nentry_in(uint8_t *bcd_score) __z88dk_fastcall;
extern uint8_t insert_score_check_key();

#endif // !__RANKING_H
