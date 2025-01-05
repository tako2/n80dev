#ifndef _BITMAP_H
#define _BITMAP_H

#include <stddef.h>
#include <stdint.h>

#define ENABLE_X_CLIPPING

#define pos_xy(x, y) ((uint16_t)((x) << 8) | (uint8_t)(y))

#define BLACK    (uint8_t)(0x18)
#define BLUE     (uint8_t)(0x18 | (1 << 5))
#define RED      (uint8_t)(0x18 | (2 << 5))
#define MAGENTA  (uint8_t)(0x18 | (3 << 5))
#define GREEN    (uint8_t)(0x18 | (4 << 5))
#define CYAN     (uint8_t)(0x18 | (5 << 5))
#define YELLOW   (uint8_t)(0x18 | (6 << 5))
#define WHITE    (uint8_t)(0x18 | (7 << 5))
#define NO_COLOR (uint8_t)(0xe8)

#define BLIT_OP_NOP  0x00
#define BLIT_OP_MASK 0x01
#define BLIT_OP_OR   0x02
#define BLIT_OP_XOR  0x03

typedef struct {
	uint8_t pos;
	uint8_t attr;
} CRTCAttr_t;

typedef union {
	uint16_t val;
	struct {
		uint8_t x;
		uint8_t y;
	};
} Point_t;

typedef union {
	struct {
		uint8_t w;
		uint8_t h;
	};
	uint16_t val;
} Size_t;

typedef struct {
	Point_t pt;
	Size_t sz;
} Rect_t;

typedef struct {
    uint8_t w;
    uint8_t h;
    const uint8_t *data;
} Bitmap_t;
#define SIZEOF_BITMAP_T 4

// Dummy Structure
typedef struct {
	Bitmap_t bmp1;
	Bitmap_t bmp2;
} Bitmap2_t;

#ifdef BMP_ENABLE_BLIT_OP
// extern void set_blit_op(uint8_t op) __z88dk_fastcall;
extern uint8_t blit_op;
#define set_blit_op(op) blit_op = (op)
#endif

extern void fill_hlines_attr(uint8_t start_line, uint8_t num_lines, uint8_t color);
extern void clear_attr_mapped(uint8_t start_line, uint8_t num_lines,
							  const uint8_t *attr_map,
							  uint8_t wait, uint8_t cycle);
extern void fill_rect_attr(uint16_t xy, uint8_t w, uint8_t h, uint8_t color);

extern void clip_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

extern uint8_t put_bitmap(uint8_t x, uint8_t y, const Bitmap_t *bitmap, uint8_t color);
extern uint8_t put_fixed_bitmap(uint8_t x, uint8_t y, const uint8_t *data, uint8_t color);
extern uint8_t put_bitmap2(uint8_t x, uint8_t y, const Bitmap_t *bitmap, uint8_t color);
extern uint8_t put_bitmap3(uint8_t x, uint8_t y, const Bitmap_t *bitmap, uint8_t color);
extern uint8_t or_bitmap(uint8_t x, uint8_t y, const Bitmap_t *bitmap, uint8_t color);

extern void draw_hlines(uint8_t y, uint8_t h, uint16_t ch);

extern void fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t ch, uint8_t color);
extern void invert_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
extern void draw_char(uint8_t x, uint8_t y, uint8_t ch, uint8_t color);
extern void draw_wchar(uint8_t x, uint8_t y, uint16_t ch, uint8_t color);
extern void draw_bytes(uint8_t x, uint8_t y, const uint8_t *bytes, uint8_t num_bytes, uint8_t color);
extern void draw_patlines(uint8_t y, uint8_t h, uint8_t skip, uint8_t color, uint8_t *pat);
extern void fill_bytes(uint8_t x, uint8_t y, uint8_t w, uint8_t ch, uint8_t attr);

void put_bitmap_vram(/* uint8_t *pat, uint16_t hw, uint8_t *vram */);

extern uint8_t put_bitmaph(uint8_t x, uint8_t y, const Bitmap_t *bitmap);
extern void put_bitmaph2(uint8_t x, uint8_t y, const Bitmap_t *bitmap);

extern void make_1px_bmps(const uint8_t *src, Bitmap_t *bmp, uint8_t *work, uint8_t w, uint8_t h, uint8_t size, uint8_t ext);
extern void make_rot_ch(const uint8_t *src, uint8_t *dest, uint8_t dir);

#endif
