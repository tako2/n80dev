// use 32-direction speed table
// 0 = right (dx =  1.00, dy =  0.00)
// 8 = down  (dx =  0.00, dy =  1.00)
// 16 = left (dx = -1.00, dy =  0.00)
// 24 = up   (dx =  0.00, dy = -1.00)

#ifndef _DIRTBL_H
#define _DIRTBL_H

#include "n80dev.h"

//=============================================================================
// sin(11.25) = 0.19509
// sin(22.5) = 0.38268
// sin(33.75) = 0.55557
// sin(45.0) = 0.70701
// sin(56.25) = 0.83147
// sin(67.5) = cos(22.5) = 0.92388
// sin(78.75) = 0.98079
#define SIN_000 0
#define SIN_112 20
#define SIN_225 38
#define SIN_337 56
#define SIN_450 71
#define SIN_562 83
#define SIN_675 92
#define SIN_787 98

#define make_speed_table(name, speed) \
const int16_t speed_ ## name ## _tbl[] = { \
	0, \
	((uint32_t)speed * SIN_112) / 100, \
	((uint32_t)speed * SIN_225) / 100, \
	((uint32_t)speed * SIN_337) / 100, \
	((uint32_t)speed * SIN_450) / 100, \
	((uint32_t)speed * SIN_562) / 100, \
	((uint32_t)speed * SIN_675) / 100, \
	((uint32_t)speed * SIN_787) / 100, \
	(uint16_t)speed, \
	((uint32_t)speed * SIN_787) / 100, \
	((uint32_t)speed * SIN_675) / 100, \
	((uint32_t)speed * SIN_562) / 100, \
	((uint32_t)speed * SIN_450) / 100, \
	((uint32_t)speed * SIN_337) / 100, \
	((uint32_t)speed * SIN_225) / 100, \
	((uint32_t)speed * SIN_112) / 100, \
	0, \
	-(((int32_t)speed * SIN_112) / 100), \
	-(((int32_t)speed * SIN_225) / 100), \
	-(((int32_t)speed * SIN_337) / 100), \
	-(((int32_t)speed * SIN_450) / 100), \
	-(((int32_t)speed * SIN_562) / 100), \
	-(((int32_t)speed * SIN_675) / 100), \
	-(((int32_t)speed * SIN_787) / 100), \
	- speed, \
	-(((int32_t)speed * SIN_787) / 100), \
	-(((int32_t)speed * SIN_675) / 100), \
	-(((int32_t)speed * SIN_562) / 100), \
	-(((int32_t)speed * SIN_450) / 100), \
	-(((int32_t)speed * SIN_337) / 100), \
	-(((int32_t)speed * SIN_225) / 100), \
	-(((int32_t)speed * SIN_112) / 100), \
}

extern uint32_t get_speed(uint8_t dir, const int16_t *table);

#endif //!_DIRTBL_H
