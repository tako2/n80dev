#include "n80dev.h"

//=============================================================================
#ifdef CRTC_WIDTH_40
#define CRTC_WIDTH 80
#define CRTC_CRT_BW 0 		// 1=BW, 0=COLOR
#define CRTC_CRT_80LINE 0	// 1=80LINE, 0=40LINE
#else
#define CRTC_WIDTH 80
#define CRTC_CRT_BW 0		// 1=BW, 0=COLOR
#define CRTC_CRT_80LINE 1	// 1=80LINE, 0=40LINE
#endif

#define CRTC_HEIGHT 25

// Cursor Blink Interval, Attribute Blink Interval
// 00=16,32 01=32,64 10=48,96 11=64,128
#define CRTC_BLINK 0b10

#define CRTC_SPLIT 0		// 0=Normal, 1=Split Line

// 00=No Blink, Underline
// 01=Blink, Underline
// 10=No Blink, Square
// 11=Blink, Square
#define CRTC_CURTYPE 0b11

#ifdef CRTC_24KHZ
#define CRTC_CHRH20 20		// 	20/24KHz=20 lines
#define CRTC_CHRH25 16		// 	25/24KHz=16 lines
#define CRTC_VBH20 2		// 	20/24KHz=2 lines
#define CRTC_VBH25 3		// 	25/24KHz=3 lines
#define CRTC_HBW20 26		// 	20/24KHz=26
#define CRTC_HBW25 26		// 	25/24KHz=26
#else
#define CRTC_CHRH20 10		// 	20/15KHz=10 lines
#define CRTC_CHRH25 8		// 	25/15KHz=8 lines
#define CRTC_VBH20 6		// 	20/15KHz=6 lines
#define CRTC_VBH25 7		// 	25/15KHz=7 lines
#define CRTC_HBW20 32		// 	20/15KHz=32
#define CRTC_HBW25 32		// 	25/15KHz=32
#endif

#if (CRTC_HEIGHT==25)
#define CRTC_CHRH	CRTC_CHRH25
#define CRTC_VBH	CRTC_VBH25
#define CRTC_HBW	CRTC_HBW25
#else
#define CRTC_CHRH	CRTC_CHRH20
#define CRTC_VBH	CRTC_VBH20
#define CRTC_HBW	CRTC_HBW20
#endif

#define CRTC_ATSC	 0b010	// 000=Monochro, 001=No Attributes, 010=Color

#define DMA2_ADDR	0xf300
#define DMA2_LEN	((CRTC_WIDTH + ATTRS_PER_LINE * 2) * CRTC_HEIGHT - 1)

uint8_t sys_port30h;

//=============================================================================
void waitVBlank() __preserves_regs(b, c, d, e, h, l, iyh, iyl) __naked
{
__asm
__waitVBlank_0:
	in		a, (0x40)
	and		a, #0x20
	jr		nz, __waitVBlank_0
__waitVBlank_1:
	in		a, (0x40)
	and		a, #0x20
	jr		z, __waitVBlank_1
	ret
__endasm;
}

//=============================================================================
void init_crtc() __naked
{
__asm
	ld		a, #0b00011001	// CRT I/F sync bit on
	out		(0x40), a

	call	_waitVBlank

	// Reset CRTC
	xor		a, a
	out		(0x51), a

	// DMAC Setting
	ld		a, #0b10000000	// DMAC: Mode Set(Reset)
	out		(0x68), a

	// Ch.2 DMA Address
	ld		a, #DMA2_ADDR & 0xff		// VRAM TOP 0xF300
	out		(0x64), a
	ld		a, #DMA2_ADDR >> 8
	out		(0x64), a

	// Ch.2 Terminal Count (DMA mode=read)
	ld		a, #DMA2_LEN & 0xff 	// (120 * CRTC_HEIGHT - 1) & 0xFF
	out		(0x65), a
	ld		a, #0x80 | (DMA2_LEN >> 8)	// 0x80 + ((120 * CRTC_HEIGHT - 1) >> 8)
	out		(0x65), a

	// CRTC: Screen Format 1
	ld		a, #0x80 + (CRTC_WIDTH - 2) // bit7: DMA Character Mode
	out		(0x50), a

	// CRTC: Screen Format 2
	ld		a, #(CRTC_BLINK << 6) + (CRTC_HEIGHT - 1)
	out		(0x50), a

	// CRTC: Screen Format 3
	ld		a, #((CRTC_SPLIT << 7) + (CRTC_CURTYPE << 5) + (CRTC_CHRH - 1))
	out		(0x50), a

	// CRTC: Scrren Format 4
	ld		a, #((CRTC_VBH - 1) << 5) + (CRTC_HBW - 2)
	out		(0x50), a

	// CRTC: Screen Format 5 (ATTR=20)
	ld		a, #(CRTC_ATSC << 5) + (ATTRS_PER_LINE - 1)
	out		(0x50), a

	// CRTC: Set Interrupt Mask
	// MN=1, ME=1
	ld		a, #0b01000011
	out		(0x51), a

	// bit1: color=0, bw=1
	// bit0: 40line=0, 80line=1
	//ld		a, #0b00000001
	ld		a, (_sys_port30h)
	and		a, #0xfc
	or		a, #(CRTC_CRT_BW << 1) + CRTC_CRT_80LINE
	out		(0x30), a
	ld		(_sys_port30h), a

	// DMAC: Mode Set
	ld		a, #0b11000100
	out		(0x68), a

	// CRTC: Start Display
	ld		a, #0b00100000
	out		(0x51), a

	call	_waitVBlank

_init_crtc_wait:
	in		a, (0x40)
	and		a, #0x20
	jr		nz, _init_crtc_wait

	ld		a, #0b00010001
	out		(0x40), a

	ret
__endasm;
}
