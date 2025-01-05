#ifndef _LINE_H
#define _LINE_H

#include <stddef.h>
#include <stdint.h>

#if 0
typedef struct {
	const uint8_t *path;
	uint8_t num_pts;
} PathList_t;
#endif

extern void draw_pixel(uint8_t x, uint8_t y);
extern void clear_pixel(uint8_t x, uint8_t y);
extern void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

extern const Point_t *draw_lines(const Point_t *pts, uint8_t num_pts);
extern void draw_path(const Point_t *pts_list, const uint8_t *num_pts);

extern void set_line_func(uint8_t draw) __z88dk_fastcall;
extern void init_line();

#endif // !_LINE_H
