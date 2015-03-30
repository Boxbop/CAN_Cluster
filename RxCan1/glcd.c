/*------------------------------------------------------------------------------
graphic lcd module (summary-glcd.h)
by JB Ivan Lee (jb.ivan.lee@gmail.com)
------------------------------------------------------------------------------*/

#include "glcd.h"
//#include "SysApi.h"
#include <avr/delay.h>
 
//internal function proto type ==============================
void GLCD_Delay_us(unsigned int i);
void GLCD_Delay_ms(unsigned int i);
void GLCD_BaseDelay(void);
void GLCD_Comm_Write(unsigned char c_val);
void GLCD_Data_Write(unsigned char d_val);
unsigned char GLCD_Data_Read(void);
unsigned char GLCD_Status_Read(void);
void GLCD_REG_SetCursorPointer(unsigned char x_adrs, unsigned char y_adrs);
void GLCD_REG_SetOffsetReg(unsigned char d_val);
void GLCD_REG_SetAddressPointer(unsigned char l_adrs, unsigned char h_adrs);
void GLCD_REG_SetTextHomeAddress(unsigned char l_adrs, unsigned char h_adrs);
void GLCD_REG_SetTextArea(unsigned char col_val);
void GLCD_REG_SetGraphicHomeAddress(unsigned char l_adrs, unsigned char h_adrs);
void GLCD_REG_SetGraphicArea(unsigned char col_val);
void GLCD_REG_SetMode(unsigned char code_val);
void GLCD_REG_DisplayMode(unsigned char code_val);
void GLCD_REG_CursorPatternSelect(unsigned char code_val);
void GLCD_REG_SetDataAutoWrite(void);
void GLCD_REG_SetDataAutoRead(void);
void GLCD_REG_SetDataAutoWrReset(void);
void GLCD_REG_SetDataAutoRdReset(void);
void GLCD_REG_DataRW(unsigned char code_val);
void GLCD_REG_ScreenPeek(void);
//unsigned char GLCD_REG_ScreenCopy(void);
void GLCD_REG_BitSetReset(unsigned char code_val);
//===========================================================

//global value ==============================================
//unsigned char image_logo[GLCD_DISP_SIZE_BYTE]={0xff,0xff};
//===========================================================

void GLCD_Delay_us(unsigned int i)
{
	_delay_us(i);
}

void GLCD_Delay_ms(unsigned int i)
{
	_delay_ms(i);
}

void GLCD_BaseDelay(void)
{
	GLCD_Delay_us(10);
}

//t6963 interface function ==================================
void GLCD_Comm_Write(unsigned char c_val)
{
	GLCD_BaseDelay();
	
	GLCD_PORT_INST |= ((1<<RS_LCD));
	GLCD_PORT_INST |=(1<<EN_LCD);	
	GLCD_PORT_DATA = c_val;
	GLCD_PORT_INST &=(~(1<<EN_LCD));
}

void GLCD_Data_Write(unsigned char d_val)
{
	GLCD_BaseDelay();
	GLCD_PORT_INST &= (~(1<<RS_LCD));
	GLCD_PORT_INST |= (1<<EN_LCD);
	GLCD_PORT_DATA = d_val;
	GLCD_PORT_INST &=(~(1<<EN_LCD));

}

unsigned char GLCD_Data_Read(void)
{
	unsigned char ret;
	DATA_LCD_DDR=0x00;
	GLCD_BaseDelay();
	ret = GLCD_PORT_DATA;
	DATA_LCD_DDR=0xff;
	return ret;
}
/* status word format
D[7:0]=STA7:STA6:STA5:STA4:STA3:STA2:STA1:STA0
STA7 : check the blink condition (0:display off,1:normal display)
STA6 : error flag. used for screen peek and screen copy commands (0:no error,1:error)
STA5 : check controller operation capablity (0:disable,1:enable)
STA4 : not used
STA3 : check auto mode data write capablity (0:disable,1:enable)
STA2 : check auto mode data read capablity (0:disable,1:enable)
STA1 : check data read/write capablity ((0:disable,1:enable)
STA0 : check command excution capablity (0:disable,1:enable)
*/
unsigned char GLCD_Status_Read(void)
{
	unsigned char ret;
	
	GLCD_BaseDelay();
	ret = GLCD_PORT_INST;
	
	return ret;
}
//===========================================================


//t6963 function set ========================================
/* position of the cursor by x,y address*/
void GLCD_REG_SetCursorPointer(unsigned char x_adrs, unsigned char y_adrs)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Data_Write(x_adrs);
	GLCD_Data_Write(y_adrs);
	GLCD_Comm_Write(GLCD_FUNC_CursorPointerSet);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(x_adrs);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(y_adrs);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Comm_Write(GLCD_FUNC_CursorPointerSet);
#endif
}

/* determine the external character generator RAM area. (refer to document of T6963C)
16bits(ad[15:0])
ad[15:11] : offset register data
ad[10:3] : character code
ad[2:0] : line scan */
void GLCD_REG_SetOffsetReg(unsigned char d_val)
{
#if 0//OPTION_USE_STATUS_CHECK
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(d_val);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(0x00);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Comm_Write(GLCD_FUNC_OffsetRegisterSet);
#else
	GLCD_Data_Write(d_val);
	GLCD_Data_Write(0x00);
	GLCD_Comm_Write(GLCD_FUNC_OffsetRegisterSet);
#endif
}

/* indicate the start address for writing to(or reading form) external RAM.*/
void GLCD_REG_SetAddressPointer(unsigned char l_adrs, unsigned char h_adrs)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Data_Write(l_adrs);
	GLCD_Data_Write(h_adrs);
	GLCD_Comm_Write(GLCD_FUNC_AddressPointerSet);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(l_adrs);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(h_adrs);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Comm_Write(GLCD_FUNC_AddressPointerSet);
#endif
}

/* starting address in the external display RAM for text fisplay*/
void GLCD_REG_SetTextHomeAddress(unsigned char l_adrs, unsigned char h_adrs)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Data_Write(l_adrs);
	GLCD_Data_Write(h_adrs);
	GLCD_Comm_Write(GLCD_FUNC_TextHomeAddressSet);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(l_adrs);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(h_adrs);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Comm_Write(GLCD_FUNC_TextHomeAddressSet);
#endif
}

/*display columns*/
void GLCD_REG_SetTextArea(unsigned char col_val)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Data_Write(col_val);
	GLCD_Data_Write(0x00);
	GLCD_Comm_Write(GLCD_FUNC_TextAreaSet);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(col_val);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(0x00);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Comm_Write(GLCD_FUNC_TextAreaSet);
#endif
}

/* the starting address of the external display RAM used for graphic display*/
void GLCD_REG_SetGraphicHomeAddress(unsigned char l_adrs, unsigned char h_adrs)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Data_Write(l_adrs);
	GLCD_Data_Write(h_adrs);
	GLCD_Comm_Write(GLCD_FUNC_GraphicHomeAddressSet);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(l_adrs);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(h_adrs);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Comm_Write(GLCD_FUNC_GraphicHomeAddressSet);
#endif
}

/* the display columns*/
void GLCD_REG_SetGraphicArea(unsigned char col_val)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Data_Write(col_val);
	GLCD_Data_Write(0x00);
	GLCD_Comm_Write(GLCD_FUNC_GraphicAreaSet);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(col_val);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(0x00);
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Comm_Write(GLCD_FUNC_GraphicAreaSet);
#endif
}

/* display mode, text attribute mode, internal/external CG mode*/
void GLCD_REG_SetMode(unsigned char code_val)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Comm_Write(code_val);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Comm_Write(code_val);
#endif
}

/*display on/off (cursor,blink,text,graphic)*/
void GLCD_REG_DisplayMode(unsigned char code_val)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Comm_Write(code_val);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Comm_Write(code_val);
#endif
}

/*cursor pattern in the range 1line to 8lines*/
void GLCD_REG_CursorPatternSelect(unsigned char code_val)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Comm_Write(code_val);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Comm_Write(code_val);
#endif
}

/* convenient for sending a full screen of data from the external display RAM
usage : GLCD_REG_SetAddressPointer + GLCD_REG_DataAutoWrite + GLCD_Data_Write + ... + GLCD_REG_SetDataAutoWrReset*/
void GLCD_REG_SetDataAutoWrite(void)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Comm_Write(GLCD_FUNC_DataAutoWrSet);
#else
	while((GLCD_Status_Read()&0x08) != 0x08)GLCD_BaseDelay();
	GLCD_Comm_Write(GLCD_FUNC_DataAutoWrSet);
#endif
}

/* convenient for reading a full screen of data from the external display RAM
usage : GLCD_REG_SetAddressPointer + GLCD_REG_SetDataAutoRead + GLCD_Data_Read + ... + GLCD_REG_SetDataAutoRdReset*/
void GLCD_REG_SetDataAutoRead(void)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Comm_Write(GLCD_FUNC_DataAutoRdSet);
#else
	while((GLCD_Status_Read()&0x04) != 0x04)GLCD_BaseDelay();
	GLCD_Comm_Write(GLCD_FUNC_DataAutoRdSet);
#endif
}

/* convenient for reading a full screen of data from the external display RAM
usage : GLCD_REG_SetAddressPointer + GLCD_REG_SetDataAutoRead + GLCD_Data_Write(or GLCD_Data_Read) + GLCD_REG_SetDataAutoReset*/
void GLCD_REG_SetDataAutoWrReset(void)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Comm_Write(GLCD_FUNC_DataAutoReset);
#else
	while((GLCD_Status_Read()&0x08) != 0x08)GLCD_BaseDelay();
	GLCD_Comm_Write(GLCD_FUNC_DataAutoReset);
#endif
}
void GLCD_REG_SetDataAutoRdReset(void)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Comm_Write(GLCD_FUNC_DataAutoReset);
#else
	while((GLCD_Status_Read()&0x04) != 0x04)GLCD_BaseDelay();
	GLCD_Comm_Write(GLCD_FUNC_DataAutoReset);
#endif
}

/*the address pointer can be automatically incremented or decremented
usage : GLCD_REG_SetAddressPointer + GLCD_Data_Write(or GLCD_Data_Read) +GLCD_REG_DataRW*/
void GLCD_REG_DataRW(unsigned char code_val)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Comm_Write(code_val);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Comm_Write(code_val);
#endif
}

/*tranfer 1byte of displayed data to the data stack (can be read from the MPU by data access)*/
void GLCD_REG_ScreenPeek(void)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Data_Write(GLCD_FUNC_ScreenPeek);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(GLCD_FUNC_ScreenPeek);
	while((GLCD_Status_Read()&0x40) == 0x40)GLCD_Delay_us(50);
#endif
}

/*copy a single raster line of data to the graphic area (refer to document of T6963C)*/
/*
unsigned char GLCD_REG_ScreenCopy(void)
{
	//not available function for this lcd module
}
*/

/*set or reset a bit of the byte specified by the address pointer*/
void GLCD_REG_BitSetReset(unsigned char code_val)
{
#if OPTION_STATUS_CHECK_DISABLE
	GLCD_Data_Write(code_val);
#else
	while((GLCD_Status_Read()&0x03) != 0x03)GLCD_BaseDelay();
	GLCD_Data_Write(code_val);
#endif
}
//===========================================================


void GLCD_Init(void)
{
	DATA_LCD_DDR = 0xff;
	CTRL_LCD_DDR |=((1<<RS_LCD)|(1<<EN_LCD)|(1<<LCD_CS1));
	LCD_CS2_DDR |= (1<<LCD_CS2);
	//
	GLCD_REG_SetTextArea(GLCD_TEXT_AREA);
	GLCD_REG_SetTextHomeAddress(GLCD_TEXT_HOME,GLCD_TEXT_HOME>>8);
	GLCD_REG_SetGraphicArea(GLCD_GRAPHIC_AREA);
	GLCD_REG_SetGraphicHomeAddress(GLCD_GRAPHIC_HOME,GLCD_GRAPHIC_HOME>>8);
	GLCD_REG_SetMode(GLCD_FUNC_ModeSet_CGRom|GLCD_FUNC_ModeSet_OR);
	GLCD_DispFillBytes(0,0,30,64,0x00);
	GLCD_REG_DisplayMode(GLCD_FUNC_DispGrap_ON);
}

void GLCD_DispLogo(unsigned char page_val)
{
	unsigned int i;
	unsigned char tmp;
	unsigned char *tmp_ptr;
	
	if(page_val == 1)tmp_ptr = glcd_img_logo;
	else if(page_val == 2)tmp_ptr = glcd_img_logo1;
	else if(page_val == 3)tmp_ptr = glcd_img_logo2;
	else return;

	GLCD_REG_SetAddressPointer(GLCD_GRAPHIC_HOME,GLCD_GRAPHIC_HOME>>8);
	GLCD_REG_SetDataAutoWrite();
	for (i = 0; i < GLCD_DISP_SIZE_BYTE; i++) 
	{
		tmp = pgm_read_byte(&tmp_ptr[i]);
		GLCD_Data_Write(tmp);
	}
	GLCD_REG_SetDataAutoWrReset();
}

/*start_x_bytes : 0~29, start_y_lines : 0~63, width_bytes : 0~30, hight_lines : 0~64*/
void GLCD_DispFillBytes(unsigned char start_x_bytes, unsigned char start_y_lines, 
						unsigned char width_bytes, unsigned char hight_lines, unsigned char byte_val)
{
	unsigned int tmp_adr;
	unsigned int i,j;

	for (i = 0; i < hight_lines; i++) 
	{
		tmp_adr = GLCD_GRAPHIC_HOME + start_x_bytes + ((start_y_lines+i)*30);
		GLCD_REG_SetAddressPointer(tmp_adr,tmp_adr>>8);
		for (j = 0; j < width_bytes; j++) 
		{
			GLCD_Data_Write(byte_val);
			GLCD_REG_DataRW(GLCD_FUNC_DataWrIncAdp);
		}
	}
}
