#include "ranking.h"

//=============================================================================
uint8_t nentry_cur;
NameEntry_t ins_nent;

//=============================================================================
#define CHECK_KEY (KEY_4 | KEY_6)
static uint8_t check_key_press_prev;
static uint8_t check_key_repeat;
static uint8_t check_key_press(uint8_t key) __z88dk_fastcall __preserves_regs(b, c, d, e, iyh, iyl) __naked
{
	key;
__asm
	ld		h, l		// H = key
	ld		a, (_check_key_press_prev)
	or		a, l
	ld		l, a		// L = key | check_key_press_prev

	and		a, #CHECK_KEY
	cp		#CHECK_KEY
	jr		nz, _check_key_press_skip1

	ld		a, h
	and		a, #CHECK_KEY
	cp		#CHECK_KEY
	jr		z, _check_key_press_skip3

	ld		a, (_check_key_repeat)
	dec		a
	jr		nz, _check_key_press_skip2

	ld		l, h
	ld		a, #5
	jr		_check_key_press_skip2

_check_key_press_skip1:
	ld		a, #15

_check_key_press_skip2:
	ld		(_check_key_repeat), a

_check_key_press_skip3:
	ld		a, h
	xor		a, #0xff
	ld		(_check_key_press_prev), a
	ret
__endasm;

#if 0
	uint8_t pressed;

	pressed = key | check_key_press_prev;
	if ((pressed & KEY_CHECK) != KEY_CHECK) {
		// first key press
		check_key_repeat = 30;
	}
	else if ((key & KEY_CHECK) != KEY_CHECK) {
		// repeat key press
		check_key_repeat --;
		if (check_key_repeat == 0) {
			check_key_repeat = 15;
			pressed = key;
		}
	}
	check_key_press_prev = key ^ 0xff;
	return (pressed);
#endif
}

//=============================================================================
static uint8_t next_char(uint8_t letter) __z88dk_fastcall __preserves_regs(b, c, d, e, iyh, iyl) __naked
{
	letter;
__asm
	ld		a, l
	ld		hl, #_next_char_tbl
	cp		#'Z'
	jr		z, _next_char_hit
	inc		hl
	cp		#'9'
	jr		z, _next_char_hit
	inc		hl
	cp		#' '
	jr		z, _next_char_hit
	inc		hl
	cp		#'!'
	jr		z, _next_char_hit
	inc		hl
	cp		#'.'
	jr		z, _next_char_hit
	inc		a
	ld		l, a
	ret
_next_char_hit:
	ld		l, (hl)
	ret
_next_char_tbl:
	.ascii	"0 !.A"
__endasm;
}

//=============================================================================
static uint8_t prev_char(uint8_t letter) __z88dk_fastcall __preserves_regs(b, c, d, e, iyh, iyl) __naked
{
	letter;
__asm
	ld		a, l
	ld		hl, #_prev_char_tbl
	cp		#'A'
	jr		z, _prev_char_hit
	inc		hl
	cp		#'.'
	jr		z, _prev_char_hit
	inc		hl
	cp		#'!'
	jr		z, _prev_char_hit
	inc		hl
	cp		#' '
	jr		z, _prev_char_hit
	inc		hl
	cp		#'0'
	jr		z, _prev_char_hit
	dec		a
	ld		l, a
	ret
_prev_char_hit:
	ld		l, (hl)
	ret
_prev_char_tbl:
	.ascii	".! 9Z"
__endasm;
}

//=============================================================================
int8_t insert_nentry_in(uint8_t *bcd_score) __z88dk_fastcall __naked
{
	bcd_score;
__asm
	// HL = &bcd_score[3]
	ld		de, #_ins_nent	// DE = &ins_nent.score
	ld		bc, #3
	ldir

	ex		de, hl
	ld		(hl), #'A'
	inc		hl
	ld		(hl), #' '
	inc		hl
	ld		(hl), #' '
	inc		hl
	ld		(hl), #0

	ld		hl, #_ins_nent
	call	_insert_nentry

	ret
__endasm;

#if 0
	for (cnt = 0; cnt < 3; cnt ++) {
		ins_nent.score[cnt] = bcd_score[cnt];
	}
	ins_nent.name[0] = 'A';
	ins_nent.name[1] = ' ';
	ins_nent.name[2] = ' ';
	ins_nent.name[3] = 0;

	rank = insert_nentry(&ins_nent);
#endif
}

//=============================================================================
uint8_t insert_score_check_key() __naked
{
__asm
	call	_in_tenkey
	bit		1, l	// KEY_X
	jr		z, _insert_score_key_x
	bit		5, l	// KEY_SPACE
	jr		z, _insert_score_key_x
	bit		3, l	// KEY_Z
	jr		z, _insert_score_key_z
	call	_check_key_press

	ld		a, (_nentry_cur)
	call	_insert_score_nentry_cur
	ld		a, (bc)
	ex		de, hl
	ld		l, a
	bit		6, e	// KEY_6
	jr		z, _insert_score_key_6
	bit		4, e	// KEY_4
	jr		z, _insert_score_key_4
	ld		l, #0
	ret

_insert_score_key_4:
	call	_prev_char
	ld		a, l
	ld		(bc), a
	ld		l, #0
	ret

_insert_score_key_6:
	call	_next_char
	ld		a, l
	ld		(bc), a
	ld		l, #0
	ret

_insert_score_key_z:
	ld		hl, #_nentry_cur
	ld		a, (hl)
	or		a, a
	jr		z, _insert_score_key_z_skip
	ld		e, a
	call	_insert_score_nentry_cur
	ld		a, #' '
	ld		(bc), a
	dec		e
	ld		(hl), e
_insert_score_key_z_skip:
	ld		l, #0
	ret

_insert_score_key_x:
	ld		hl, #_nentry_cur
	ld		a, (hl)
#if 0
	inc		a
	ld		(hl), a
	cp		#3
#else
	cp		#2
#endif
	jr		z, _insert_score_key_x_end
	call	_insert_score_nentry_cur
#if 0
	ld		a, #'A'
	ld		(bc), a
#else
	inc		(hl)
	ld		a, (bc)
	inc		bc
	ld		(bc), a
#endif
	ld		l, #0
	ret

_insert_score_key_x_end:
	ld		l, #1
	ret

_insert_score_nentry_cur:
	// BC = ins_nent.name[cur]
	add		a, #3
	add		a, #<_ins_nent
	ld		c, a
	adc		a, #>_ins_nent
	sub		a, c
	ld		b, a
	ret
__endasm;
#if 0
	uint8_t key = in_tenkey();

	if ((key & KEY_X) == 0) {
		nentry_cur ++;
		if (nentry_cur == 3) return (1);
		ins_nent.name[nentry_cur] = 'A';
	}
	if ((key & KEY_Z) == 0) {
		if (nentry_cur > 0) {
			ins_nent.name[nentry_cur] = ' ';
			nentry_cur --;
		}
	}
	key = check_key_press(key);
	if ((key & KEY_6) == 0) {
		ins_nent.name[nentry_cur] = next_char(ins_nent.name[nentry_cur]);
	}
	if ((key & KEY_4) == 0) {
		ins_nent.name[nentry_cur] = prev_char(ins_nent.name[nentry_cur]);
	}
	return (0);
#endif
}
