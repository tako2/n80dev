#include "mp3play.h"

#define UARTD	0x20
#define UARTC	0x21
#define UART_TIMEOUT	0x1000

#define TXRDY	0x01
#define RXRDY	0x02

#define SYS_LAST30H	0xEA66
extern uint8_t sys_port30h;

//=============================================================================
void uart_reset(uint8_t mode) __naked
{
	mode;
__asm
	ld		iy, #0
	add		iy, sp

	xor		a, a
	ld		c, #UARTC
	out		(c), a
	out		(c), a
	out		(c), a
	ld		a, #0b01000000
	out		(c), a
	ld		a, 2 (iy)
	out		(c), a
	ld		a, #0b00110111
	out		(c), a

	ld		a, (SYS_LAST30H)
	and		a, #0b11001111
	or		a, #0b00100000
	ld		(_sys_port30h), a
	out		(0x30), a

	ret
__endasm;
}

//=============================================================================
void uart_send(const uint8_t *data, uint8_t len) __naked
{
	data; len;
__asm
#if 0
	ld		iy, #0
	add		iy, sp

	ld		l, 2 (iy)
	ld		h, 3 (iy)
	ld		b, 4 (iy)
#endif

	// HL = *data
	// B = len

	ld		c, #UARTD
_uart_send_loop3:
	ld		de, #UART_TIMEOUT
_uart_send_loop2:
	dec		de
	ld		a, d
	or		a, e
	jr		nz, _uart_send_skip
	ld		l, #1	// TIMEOUT
	ret
_uart_send_skip:
	in		a, (UARTC)
	and		a, #TXRDY
	jr		z, _uart_send_loop2
	outi
	jr		nz, _uart_send_loop3
	ld		l, #0
	ret
__endasm;
}

//=============================================================================
#define DFP_CMD_MEDIA	0x09	// 再生するデバイスを選択する
#define DFP_CMD_RESET	0x0C	// チップをリセットする
#define DFP_CMD_PLAY	0x0D	// 再生を開始する
#define DFP_CMD_PAUSE	0x0E	// 一時停止する
#define DFP_CMD_PGM		0x0F	// フォルダとトラック番号を指定して再生する
#define DFP_CMD_STOP	0x16	// 再生を停止する
#define DFP_CMD_REPEAT	0x19	// 現在再生中のトラックを繰り返し再生する
#define DFP_CMD_ADVERT	0x13	// [ADVERT]フォルダの中のトラックを割り込みで再生する
#define DFP_SIZE		10		// コマンドバイト列のサイズ

//=============================================================================
#if 0
static const uint8_t template[10] = {
	0x7e, 0xff, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xef,
};
#endif
uint8_t mp3_send_cmd(uint8_t cmd, uint8_t param1, uint8_t param2) __naked
{
	cmd; param1; param2;
__asm
	ld		iy, #0
	add		iy, sp

	ld		a, 2 (iy)
	ld		e, 3 (iy)
	ld		d, 4 (iy)

	LD	(.TEMPLATE+3),A
	LD	(.TEMPLATE+5),DE

	//チェックサムを求める
	//+1バイト目から6バイトが対象
	LD	DE,#.TEMPLATE+1
	LD	HL, #0x0000
	LD	B,#6			//データ長
.L1:	LD	A,(DE)
	INC	DE
		ADD	A,L			//HL+=A
	LD	L,A
	ADC	A,H
	SUB	L
	LD	H,A
	DJNZ	.L1
	EX	DE,HL
	LD	HL,#0x0000
	OR	A
	SBC	HL,DE			//HL=チェックサム

	LD	A,H
	LD	H,L
	LD	L,A
	LD	(.TEMPLATE+7),HL

	LD	HL,#.TEMPLATE
	LD	B,#DFP_SIZE
	JP	_uart_send

	// 送信フォーマットのテンプレート
	// +3,+5,+6,+7,+8バイト目を置き換えて送信する
	// 
	//  +0バイト目=開始コード($7E)
	//  +1バイト目=バージョン($FF)
	//  +2バイト目=データ長($06)
	//  +3バイト目=コマンド
	//  +4バイト目=フィードバックフラグ($00=なし,$01=あり)
	//  +5バイト目=パラメータ上位バイト
	//  +6バイト目=パラメータ下位バイト
	//  +7バイト目=チェックサム上位バイト
	//  +8バイト目=チェックサム下位バイト
	//  +9バイト目=終了コード($EF)
.TEMPLATE:
	.db	0x7E,0xFF,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0xEF
__endasm;

#if 0
	uint8_t *work;
	uint16_t sum;
	uint8_t cnt;

	work = (uint8_t *)template;

	work[3] = cmd;
	work[5] = param1;
	work[6] = param2;

	sum = 0;
	work ++;
	for (cnt = 6; cnt != 0; cnt --) {
		sum += *work;
		work ++;
	}
	sum = 0 - sum;
	*work ++ = sum >> 8;
	*work  = sum;

	uart_send(template, 10);
#endif
}

//=============================================================================
void mp3_reset()
{
	uart_reset(0b01001101);

	mp3_send_cmd(DFP_CMD_RESET, 0, 0);

	wait_loop(13); // wait at least 200ms
}

//=============================================================================
void mp3_media(uint8_t media)
{
	mp3_send_cmd(DFP_CMD_MEDIA, 0, media);

	wait_loop(13); // wait at least 200ms
}

//=============================================================================
void mp3_pgm(uint8_t folder, uint8_t track)
{
	mp3_send_cmd(DFP_CMD_PGM, folder, track);
}

//=============================================================================
void mp3_play()
{
	mp3_send_cmd(DFP_CMD_PLAY, 0, 0);
}

//=============================================================================
void mp3_pause()
{
	mp3_send_cmd(DFP_CMD_PAUSE, 0, 0);
}

//=============================================================================
void mp3_repaet()
{
	// 曲の再生が始まってから設定しないと反映しないので注意
	mp3_send_cmd(DFP_CMD_REPEAT, 0, 0);
}

//=============================================================================
void mp3_stop()
{
	mp3_send_cmd(DFP_CMD_STOP, 0, 0);
}

//=============================================================================
void mp3_advert(uint16_t track)
{
	mp3_send_cmd(DFP_CMD_ADVERT, track >> 8, track & 0xff);
}
