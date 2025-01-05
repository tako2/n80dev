#ifndef _INSERT_ATTR_H
#define _INSERT_ATTR_H

#include <stddef.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
typedef struct _attr_list_t {
	uint8_t x;
	uint8_t attr;
	struct _attr_list_t *next;
} AttrList_t;

extern AttrList_t mAttrList[25];

//-----------------------------------------------------------------------------
extern void insert_attr(uint8_t line, uint8_t x, uint8_t w, uint8_t attr);
extern void clear_attr(uint8_t attr) __z88dk_fastcall;
extern void clear_attr_mapped2(const uint8_t *table) __z88dk_fastcall;
extern void render_attr(
#ifdef BMP_USE_RENDER_ATTR_PARAM
						uint8_t y, uint8_t h
#endif
);
#ifdef BMP_USE_INSERT_ATTR_RIGHT
extern void insert_attr_right(uint8_t line, uint8_t x, uint8_t attr);
#endif
#ifdef BMP_USE_IMPORT_ATTR
extern void import_attr(uint8_t y, uint8_t h);
#endif
extern void append_attr(const uint8_t *table) __z88dk_fastcall;
extern void small_beep_on();
extern void small_beep_off();

extern void insert_char_attr(uint8_t line, uint8_t x, uint8_t attr);
extern void insert_char_attr_reg();

extern AttrList_t *alloc_attr(uint32_t attr_x) __z88dk_fastcall;
// L = x, H = attr, DE = next

extern void enter_insert_attr2();
extern void exit_insert_attr2();
extern void insert_attr2(uint8_t line, uint8_t x, uint8_t w, uint8_t attr);

#endif // !_INSERT_ATTR_H
