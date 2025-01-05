#include <stdint.h>

#if 0
const uint8_t font_array[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 
	0xee, 0x00, 0x14, 0x01, 0x00, 0x06, 0x06, 0x00, 
	0x00, 0x00, 0x40, 0x4e, 0x4e, 0x10, 0x13, 0x13, 
	0x40, 0xea, 0x2a, 0x20, 0x32, 0x12, 0x60, 0x86, 
	0x24, 0x20, 0x01, 0x33, 0x40, 0xea, 0x80, 0x10, 
	0x22, 0x21, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xc0, 0x02, 0x00, 0x10, 0x02, 0x00, 0xc2, 
	0x00, 0x00, 0x12, 0x00, 0x20, 0xe4, 0x24, 0x20, 
	0x31, 0x21, 0x80, 0xe8, 0x88, 0x00, 0x30, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x80, 0x88, 
	0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x20, 0x00, 0x00, 0x80, 0x24, 0x20, 0x01, 0x00, 
	0xc0, 0x2e, 0xce, 0x10, 0x23, 0x13, 0x40, 0xee, 
	0x00, 0x20, 0x33, 0x02, 0x40, 0xaa, 0x4e, 0x30, 
	0x23, 0x22, 0x20, 0xaa, 0x6e, 0x20, 0x22, 0x13, 
	0xe0, 0x0e, 0xee, 0x10, 0x11, 0x33, 0xe0, 0xae, 
	0x2a, 0x20, 0x22, 0x13, 0xc0, 0xae, 0x8a, 0x10, 
	0x23, 0x13, 0x60, 0x26, 0x6e, 0x00, 0x30, 0x03, 
	0x40, 0xae, 0x4e, 0x10, 0x23, 0x13, 0xc0, 0xae, 
	0xce, 0x00, 0x22, 0x13, 0x00, 0x40, 0x00, 0x00, 
	0x10, 0x00, 0x00, 0x40, 0x00, 0x00, 0x12, 0x00, 
	0x80, 0x44, 0x22, 0x00, 0x11, 0x22, 0x40, 0x44, 
	0x44, 0x10, 0x11, 0x11, 0x20, 0x42, 0x84, 0x20, 
	0x12, 0x01, 0x40, 0x22, 0x4a, 0x00, 0x50, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x2e, 
	0xce, 0x30, 0x13, 0x33, 0xe0, 0xae, 0x4e, 0x30, 
	0x23, 0x13, 0xc0, 0x2e, 0x66, 0x10, 0x23, 0x33, 
	0xe0, 0x2e, 0xce, 0x30, 0x23, 0x13, 0xe0, 0xae, 
	0x2a, 0x30, 0x23, 0x22, 0xe0, 0xae, 0x2a, 0x30, 
	0x03, 0x00, 0xc0, 0x2e, 0xaa, 0x10, 0x23, 0x33, 
	0xe0, 0x8e, 0xee, 0x30, 0x03, 0x33, 0x20, 0xee, 
	0x02, 0x20, 0x33, 0x02, 0x00, 0xe2, 0x2e, 0x30, 
	0x32, 0x01, 0xe0, 0x8e, 0x6e, 0x30, 0x03, 0x33, 
	0xe0, 0x0e, 0x00, 0x30, 0x23, 0x22, 0xe0, 0x8c, 
	0xec, 0x30, 0x03, 0x33, 0xe0, 0x8c, 0xee, 0x30, 
	0x03, 0x31, 0xc0, 0x2e, 0xce, 0x10, 0x23, 0x13, 
	0xe0, 0x2e, 0xce, 0x30, 0x13, 0x01, 0xc0, 0x2e, 
	0xce, 0x10, 0x23, 0x21, 0xe0, 0x2e, 0xce, 0x30, 
	0x13, 0x23, 0xc0, 0xae, 0xaa, 0x20, 0x22, 0x13, 
	0x22, 0xee, 0x22, 0x00, 0x33, 0x00, 0xe0, 0x0e, 
	0xee, 0x10, 0x23, 0x13, 0xe0, 0x0e, 0xee, 0x00, 
	0x31, 0x01, 0xe0, 0x8e, 0xee, 0x30, 0x01, 0x31, 
	0x60, 0x8e, 0x6e, 0x30, 0x03, 0x33, 0x60, 0x8e, 
	0x6e, 0x00, 0x30, 0x00, 0x20, 0xe2, 0x26, 0x20, 
	0x33, 0x22, 0x00, 0xee, 0x02, 0x00, 0x33, 0x02, 
	0x20, 0x84, 0x00, 0x00, 0x00, 0x21, 0x00, 0xe2, 
	0x0e, 0x00, 0x32, 0x03, 0x80, 0x24, 0x84, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x22, 0x22, 
};
#else
const uint8_t font_array[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
	0xee, 0x00, 0x14, 0x01, 0x00, 0x06, 0x06, 0x00,
	0x00, 0x00, 0x40, 0x4e, 0x4e, 0x10, 0x13, 0x13,
	0x40, 0xea, 0x2a, 0x20, 0x32, 0x12, 0x60, 0x86,
	0x24, 0x20, 0x01, 0x33, 0x40, 0xea, 0x80, 0x10,
	0x22, 0x21, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xc0, 0x02, 0x00, 0x10, 0x02, 0x00, 0xc2,
	0x00, 0x00, 0x12, 0x00, 0x20, 0xe4, 0x24, 0x20,
	0x31, 0x21, 0x80, 0xe8, 0x88, 0x00, 0x30, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x80, 0x88,
	0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x80, 0x24, 0x20, 0x01, 0x00,
	0xc0, 0x2e, 0xce, 0x10, 0x23, 0x13, 0x40, 0xee,
	0x00, 0x20, 0x33, 0x02, 0x40, 0xaa, 0x4e, 0x30,
	0x23, 0x22, 0x20, 0xaa, 0x6e, 0x20, 0x22, 0x13,
	0xe0, 0x0e, 0xee, 0x10, 0x11, 0x33, 0xe0, 0xae,
	0x2a, 0x20, 0x22, 0x13, 0xc0, 0xae, 0x8a, 0x10,
	0x23, 0x13, 0x60, 0x26, 0x6e, 0x00, 0x30, 0x03,
	0x40, 0xae, 0x4e, 0x10, 0x23, 0x13, 0xc0, 0xae,
	0xce, 0x00, 0x22, 0x13, 0x00, 0x40, 0x00, 0x00,
	0x10, 0x00, 0x00, 0x40, 0x00, 0x00, 0x12, 0x00,
	0x80, 0x44, 0x22, 0x00, 0x11, 0x22, 0x40, 0x44,
	0x44, 0x10, 0x11, 0x11, 0x20, 0x42, 0x84, 0x20,
	0x12, 0x01, 0x40, 0x22, 0x4a, 0x00, 0x50, 0x00,
	0xc0, 0xa2, 0xca, 0x10, 0x12, 0x32, 0xc0, 0x2e,
	0xce, 0x30, 0x13, 0x33, 0xe0, 0xae, 0x4e, 0x30,
	0x23, 0x13, 0xc0, 0x2e, 0x66, 0x10, 0x23, 0x33,
	0xe0, 0x2e, 0xce, 0x30, 0x23, 0x13, 0xe0, 0xae,
	0x2a, 0x30, 0x23, 0x22, 0xe0, 0xae, 0x2a, 0x30,
	0x03, 0x00, 0xc0, 0x2e, 0xaa, 0x10, 0x23, 0x33,
	0xe0, 0x8e, 0xee, 0x30, 0x03, 0x33, 0x20, 0xee,
	0x02, 0x20, 0x33, 0x02, 0x00, 0xe2, 0x2e, 0x30,
	0x32, 0x01, 0xe0, 0x8e, 0x6e, 0x30, 0x03, 0x33,
	0xe0, 0x0e, 0x00, 0x30, 0x23, 0x22, 0xe0, 0x8c,
	0xec, 0x30, 0x03, 0x33, 0xe0, 0x8c, 0xee, 0x30,
	0x03, 0x31, 0xc0, 0x2e, 0xce, 0x10, 0x23, 0x13,
	0xe0, 0x2e, 0xce, 0x30, 0x13, 0x01, 0xc0, 0x2e,
	0xce, 0x10, 0x23, 0x21, 0xe0, 0x2e, 0xce, 0x30,
	0x13, 0x23, 0xc0, 0xae, 0xaa, 0x20, 0x22, 0x13,
	0x22, 0xee, 0x22, 0x00, 0x33, 0x00, 0xe0, 0x0e,
	0xee, 0x10, 0x23, 0x13, 0xe0, 0x0e, 0xee, 0x00,
	0x31, 0x01, 0xe0, 0x8e, 0xee, 0x30, 0x01, 0x31,
	0x60, 0x8e, 0x6e, 0x30, 0x03, 0x33, 0x60, 0x8e,
	0x6e, 0x00, 0x30, 0x00, 0x20, 0xe2, 0x26, 0x20,
	0x33, 0x22, 0x00, 0xee, 0x02, 0x00, 0x33, 0x02,
	0xc2, 0xec, 0x4e, 0x20, 0x33, 0x01, 0x00, 0xe2,
	0x0e, 0x00, 0x32, 0x03, 0x80, 0x24, 0x84, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x22, 0x22,
};
#endif
