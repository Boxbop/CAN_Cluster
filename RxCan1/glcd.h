/*------------------------------------------------------------------------------
LCD MODULE SUMMARY (www.FindLCD.com)
- no : G240064A14-BIW-R
- format : 240x64 dots
- mode : STN/Negative Transmissive mode/Blue
- Viewing direction : 6 o'clock
- control ic : T6963
- dot pitch : 0.53x0.53 mm
- dot size : 0.49x0.49 mm

HARDWARE CONFIGURATION
- JUMPER J6 for VEE : LCD PIN9 solder short
- RESET circuit : 10Kohm + 0.1uF
- Font Selection terminal (pin19) : ground = 8x8 (if vcc = 5x8)

by JB Ivan Lee (jb.ivan.lee@gmail.com)
------------------------------------------------------------------------------*/

#ifndef _GLCD_H_
#define _GLCD_H_

#include "lcd_data.h"

/*interface assignment macro*/
/*=========================================*/
//#define GLCD_PORT_DATA		(*(unsigned char *) (0xf800))	//GLCD DATA Address (C/D=a0=0)
//#define GLCD_PORT_INST		(*(unsigned char *) (0xf801))	//GLCD INSTRUCTION Address (C/D=a0=1)
#if 0
#define GLCD_PORT_DATA PORTB
#define GLCD_PORT_INST PORTE
#define LCD_CS1	5
#define LCD_CS2  7
#define LCD_CS2_PORT PORTC
#define RS_LCD 6
#define WR_LCD 0
#define EN_LCD 7

#define DATA_LCD_DDR DDRB
#define CTRL_LCD_DDR DDRE
#define LCD_CS2_DDR DDRC
#else


#define GLCD_PORT_DATA PORTC
#define GLCD_PORT_INST PORTG
#define LCD_CS1	2
#define LCD_CS2  3
#define LCD_CS2_PORT PORTG
#define RS_LCD 0
#define WR_LCD 0
#define EN_LCD 1

#define DATA_LCD_DDR DDRC
#define CTRL_LCD_DDR DDRG
#define LCD_CS2_DDR DDRG
#endif
/*=========================================*/

/*system definition*/
/*=========================================*/
#define OPTION_STATUS_CHECK_DISABLE			1		//0:check status, 1:insert delay
/*=========================================*/

/*instruction macro*/	
/*=========================================*/
//register setting
#define GLCD_FUNC_CursorPointerSet			0x21
#define GLCD_FUNC_OffsetRegisterSet			0x22
#define GLCD_FUNC_AddressPointerSet			0x24
//set control word
#define GLCD_FUNC_TextHomeAddressSet		0x40
#define GLCD_FUNC_TextAreaSet					0x41
#define GLCD_FUNC_GraphicHomeAddressSet	0x42
#define GLCD_FUNC_GraphicAreaSet				0x43
//mode set
#define GLCD_FUNC_ModeSet_OR					0x80
#define GLCD_FUNC_ModeSet_EXOR				0x81
#define GLCD_FUNC_ModeSet_AND					0x83
#define GLCD_FUNC_ModeSet_TextOnly			0x84
#define GLCD_FUNC_ModeSet_CGRom				0x80
#define GLCD_FUNC_ModeSet_CGRam				0x88
//display mode
#define GLCD_FUNC_DispAll_OFF					0x90
#define GLCD_FUNC_DispGrap_ON					0x98
#define GLCD_FUNC_DispTxt_ON					0x94
#define GLCD_FUNC_DispCsr_ON					0x92
#define GLCD_FUNC_DispBlk_ON					0x91
//cursor pattern select
#define GLCD_FUNC_CursorBottom				0xa0
#define GLCD_FUNC_Cursor2Line					0xa1
#define GLCD_FUNC_Cursor3Line					0xa2
#define GLCD_FUNC_Cursor4Line					0xa3
#define GLCD_FUNC_Cursor5Line					0xa4
#define GLCD_FUNC_Cursor6Line					0xa5
#define GLCD_FUNC_Cursor7Line					0xa6
#define GLCD_FUNC_CursorBlock					0xa7
//data auto read/write
#define GLCD_FUNC_DataAutoWrSet				0xb0
#define GLCD_FUNC_DataAutoRdSet				0xb1
#define GLCD_FUNC_DataAutoReset				0xb2
//data read/write
#define GLCD_FUNC_DataWrIncAdp				0xc0
#define GLCD_FUNC_DataRdIncAdp				0xc1
#define GLCD_FUNC_DataWrDecAdp				0xc2
#define GLCD_FUNC_DataRdDecAdp				0xc3
#define GLCD_FUNC_DataWrNonAdp				0xc4
#define GLCD_FUNC_DataRdNonAdp				0xc5
//screen peek
#define GLCD_FUNC_ScreenPeek					0xe0
//screen copy
#define GLCD_FUNC_CopyDispLine				0xe8
//bit set/reset
#define GLCD_FUNC_Bit_Reset					0xf0
#define GLCD_FUNC_Bit_Set						0xf8
#define GLCD_FUNC_Bit_0							0xf0
#define GLCD_FUNC_Bit_1							0xf1
#define GLCD_FUNC_Bit_2							0xf2
#define GLCD_FUNC_Bit_3							0xf3
#define GLCD_FUNC_Bit_4							0xf4
#define GLCD_FUNC_Bit_5							0xf5
#define GLCD_FUNC_Bit_6							0xf6
#define GLCD_FUNC_Bit_7							0xf7
/*=========================================*/

/*GLCD memory properties*/
/*=========================================*/
#define GLCD_DISP_SIZE_BYTE			1024

#define GLCD_NUMBER_OF_LINES			128
#define GLCD_PIXELS_PER_LINE			240
#define GLCD_FONT_WIDTH					8

#define GLCD_GRAPHIC_AREA				(GLCD_PIXELS_PER_LINE / GLCD_FONT_WIDTH)
#define GLCD_TEXT_AREA					(GLCD_PIXELS_PER_LINE / GLCD_FONT_WIDTH)
#define GLCD_GRAPHIC_SIZE				(GLCD_GRAPHIC_AREA * GLCD_NUMBER_OF_LINES)
#define GLCD_TEXT_SIZE					(GLCD_TEXT_AREA * (GLCD_NUMBER_OF_LINES/8))

#define GLCD_TEXT_HOME					0
#define GLCD_GRAPHIC_HOME				(GLCD_TEXT_HOME + GLCD_TEXT_SIZE)
#define GLCD_OFFSET_REGISTER			2
#define GLCD_EXTERNAL_CG_HOME			(GLCD_OFFSET_REGISTER << 11)

#define GLCD_DISP_STR16X19_LEN_MAX		15
#define GLCD_DISP_STR8X11_LEN_MAX		30
/*=========================================*/

/*general definition*/
/*=========================================*/
#define GLCD_OK			1
#define GLCD_ERR			0

#define GLCD_TRUE			1
#define GLCD_FALSE		0

#define GLCD_HIGH			1
#define GLCD_LOW			0

#define GLCD_ON			1
#define GLCD_OFF			0
/*=========================================*/


//function proto type ============================================
void GLCD_Init(void);
void GLCD_DispLogo(unsigned char page_val);
void GLCD_DispFillBytes(unsigned char start_x_bytes, unsigned char start_y_lines, unsigned char width_bytes, 
								unsigned char hight_lines, unsigned char byte_val);
//================================================================

#endif //_GLCD_H_
