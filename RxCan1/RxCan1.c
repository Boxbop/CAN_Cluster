#include <avr/io.h>
#include <stdio.h>
#include <avr/delay.h>
#include <avr/interrupt.h>


#include "myCANLib.h"
#include "usartLib.h"
#include "asc_font.h"
#include "glcd.h"


//
#define FREQ_CLKIO 		16000000
#define PRESCAIL 		64
#define RC_SERVO_PERIOD	25
//
// glcd define
#define LCD_WRITE_DATA 1
#define LCD_WRITE_CMD	0
#define LCD_CS1_ON		1
#define LCD_CS2_ON		2
#define LCD_DISP_ON		0x3f
#define LCD_DISP_OFF	0x3e
#define LCD_XADDR		0x40
#define LCD_PAGE		0xb8
#define LCD_WIDTH		128
#define LCD_HEIGHT		64
#define LCD_IMG_CNV		1
#define LCD_IMG_NCNV	0
//
struct MOb msg1;
struct MOb msg2={	0x020,
					0,
					EXT,
					8,
					{0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38}};
//
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                       /* Unsigned  8 bit quantity                         */
typedef signed   char  INT8S;                       /* Signed    8 bit quantity                         */
typedef unsigned int   INT16U;                      /* Unsigned 16 bit quantity                         */
typedef signed   int   INT16S;                      /* Signed   16 bit quantity                         */
typedef unsigned long  INT32U;                      /* Unsigned 32 bit quantity                         */
typedef signed   long  INT32S;                      /* Signed   32 bit quantity                         */
typedef float          FP32;    


//

unsigned char flg_can;
SIGNAL(CANIT_vect)
{
	char rece_result = _RECE_FAIL;
	unsigned char i;
	unsigned long can_id=0;
	rece_result = _RECE_OK;
	if((CANCDMOB & 0x10) == 0x00)
	{
		msg1.ide = STD;
		can_id = ((unsigned long)CANIDT1)<<8;
		can_id |= ((unsigned long) CANIDT2);
		can_id >>=5;
//		usart1_transmit_string("\rRx standart\n");
	}
	else
	{
		msg1.ide= EXT;
		can_id=((unsigned long)CANIDT1)<<24;
		can_id |= ((unsigned long)CANIDT2)<<16;
		can_id |= ((unsigned long)CANIDT3)<<8;
		can_id |= ((unsigned long)CANIDT4);
		can_id>>=3;
//		usart1_transmit_string("\rRx Extended\n");
	}
	msg1.id=can_id;
	msg1.dlc = CANCDMOB & 0x0f;
	for(i=0;i<(CANCDMOB&0x0f);i++)
	{
		msg1.data[i]=CANMSG;
	}
	flg_can=1;
	CANSTMOB = 0;
	CANCDMOB |= (1<<CONMOB1);
	CANSTMOB &= ~(1<<RXOK);
}
void initPort()
{
	DDRB = 0xf1;
	DDRA |=(1<<6);
	//DDRC = 0xff;
	//PORTC = 0x7f;
	DDRD = 0x3b;
	//DDRG = 0xff;
	//PORTG = 0x00;
}

void initRcServoMotor(void)
{
	TCCR1A = 0xaa;
	TCCR1B = 0x13;
	ICR1 = FREQ_CLKIO/2/PRESCAIL/1000*RC_SERVO_PERIOD;
	OCR1A = 0x70;
	OCR1B = 0x70;
	OCR1C = 0x70;
}

void CtrlRcServoMotor(unsigned char id,unsigned int degree)
{
	switch(id)
	{
		case 0: OCR1A = FREQ_CLKIO/2/PRESCAIL/1000*(1.6/180*degree + (0.15/180*degree+0.6));
		break;
		case 1: OCR1B = FREQ_CLKIO/2/PRESCAIL/1000*(1.6/180*degree + (0.15/180*degree+0.6));
		break;
		case 2: OCR1C = FREQ_CLKIO/2/PRESCAIL/1000*(1.6/180*degree + (0.15/180*degree+0.6));
		break;
		default : break;
	}
}
// glcd function
INT8U GLCD_data[128][8];

void PortInit(void)
{
	DATA_LCD_DDR = 0xff;
	CTRL_LCD_DDR |=((1<<RS_LCD)|(1<<EN_LCD)|(1<<LCD_CS1));
	LCD_CS2_DDR |= (1<<LCD_CS2);
}
void GLCD_Handler(INT8U rs,INT8U cs,INT8U data)
{
	_delay_us(100);
	GLCD_PORT_INST &=(~((1<<RS_LCD) | (1<<EN_LCD) | (1<<LCD_CS1)));
	LCD_CS2_PORT &= (~(1<<LCD_CS2));
	//GLCD_PORT_INST &=(~(1<<EN_LCD));
	if(rs == LCD_WRITE_DATA) GLCD_PORT_INST |= (1<<RS_LCD);
	else GLCD_PORT_INST &=(~(1<<RS_LCD));
	if(cs & LCD_CS1_ON) GLCD_PORT_INST |= (1<<LCD_CS1);
	else GLCD_PORT_INST &=(~(1<<LCD_CS1));
	if(cs & LCD_CS2_ON) LCD_CS2_PORT |= (1<<LCD_CS2);
	else LCD_CS2_PORT &=(~(1<<LCD_CS2));
	GLCD_PORT_INST |= (1<<EN_LCD);
	GLCD_PORT_DATA = data;
	//_delay_us(10);
	GLCD_PORT_INST &=(~(1<<EN_LCD));
}

void GLCD_Init_1(void)
{
	PortInit();
	GLCD_Handler(LCD_WRITE_CMD,LCD_CS1_ON | LCD_CS2_ON,0x3f);
	//_delay_ms(1);
	GLCD_Handler(LCD_WRITE_CMD,LCD_CS1_ON | LCD_CS2_ON,0xc0);
	//_delay_ms(1);
}

void GLCD_Clear(void)
{
	INT16U i,j;
	for(j=0;j<64;j++)
	{
		for(i=0;i<8;i++)
		{
			GLCD_data[j][i]=0;
			GLCD_data[64+j][i]=0;
			GLCD_Handler(LCD_WRITE_CMD,LCD_CS1_ON | LCD_CS2_ON,LCD_PAGE|i);
			GLCD_Handler(LCD_WRITE_CMD,LCD_CS1_ON | LCD_CS2_ON,LCD_XADDR|j);
			GLCD_Handler(LCD_WRITE_DATA,LCD_CS1_ON | LCD_CS2_ON,0);
		}
	}
}

void GLCD_Pixel(INT16U col,INT16U row,INT8U data)
{
	INT16U page,xaddr,cs,yaddr;
	if(col>=0 && col<128 && row>=0 && row <64)
	{
		cs=col<64 ? LCD_CS1_ON: LCD_CS2_ON;
		page = row/8;
		xaddr =row%8;
		yaddr = col%64;
		if(data==0) GLCD_data[col][page]=GLCD_data[col][page] & ~(0x01<<xaddr);
		else GLCD_data[col][page]=GLCD_data[col][page] | (0x01<<xaddr);
	}
	GLCD_Handler(LCD_WRITE_CMD,cs,LCD_PAGE|page);
	GLCD_Handler(LCD_WRITE_CMD,cs,LCD_XADDR|yaddr);
	GLCD_Handler(LCD_WRITE_DATA,cs,GLCD_data[col][page]);
}
void GLCD_ClearBlock(INT16U leftx,INT16U topy,INT16U rightx,INT16U boty,INT8U cnv)
{
	INT16U cntx,cnty;
	for(cnty=topy;cnty<boty;cnty++)
	{
		for(cntx=leftx;cntx<rightx;cntx++)
		{
			GLCD_Pixel(cntx,cnty,cnv);
		}
	}
}

void GLCD_DispImag(INT16U page_val,INT8U cnv)
{
	unsigned int i,j;
	unsigned char tmp;
	unsigned char *tmp_ptr;
	INT8U yaddr,page,cs,xaddr;
	if(page_val == 1)tmp_ptr = glcd_img_logo;
	else if(page_val == 2)tmp_ptr = glcd_img_logo1;
	else if(page_val == 3)tmp_ptr = glcd_img_logo2;
	else if(page_val == 4) tmp_ptr = glcd_img_logo3;
	else return;
	for(i=0;i<1920;i++)
	{
		yaddr=i%30;
		xaddr=i/30;
		if(yaddr<15)
		{
			tmp = pgm_read_byte(&tmp_ptr[i]);
			for(j=0;j<8;j++)
			{
				if(cnv) GLCD_Pixel(yaddr*8+j,xaddr,~(tmp<<j)&0x80);
				else GLCD_Pixel(yaddr*8+j,xaddr,(tmp<<j)&0x80);
			}
			
		}
	
	}
	#if 0
	for (i = 0; i < 1920; i++) 
	{
	
		yaddr=i%240;
		if(yaddr<128)
		{
			tmp = pgm_read_byte(&tmp_ptr[i]);
			page = i/240;
			cs=yaddr<64 ? LCD_CS1_ON: LCD_CS2_ON;
			yaddr%=64;
			GLCD_Handler(LCD_WRITE_CMD,cs,LCD_PAGE|page);
			GLCD_Handler(LCD_WRITE_CMD,cs,LCD_XADDR|yaddr);
			GLCD_Handler(LCD_WRITE_DATA,cs,tmp);
		}
	}
	#endif
}
void GLCD_DispFont(INT16U col,INT16U row,INT8U Ascii,INT8U cnv)
{
	unsigned int i,j;
	unsigned char tmp;
	unsigned char *tmp_ptr;
	INT8U yaddr,page,cs,xaddr;
	if(Ascii>='A' && Ascii<='Z')
	{
		tmp_ptr = nFontBicAlp;
		for(i=0;i<FONT_SIZE;i++)
		{
			tmp = pgm_read_byte(&tmp_ptr[(Ascii-'A')*FONT_SIZE+i]);
			for(j=0;j<8;j++)
			{
				if(cnv) GLCD_Pixel(col+j,row+i,~(tmp>>j)&0x01);
				else GLCD_Pixel(col+j,row+i,(tmp>>j)&0x01);
			}
		}

	}
	if(Ascii>='0' && Ascii<='9')
	{
		tmp_ptr = nFontNum;
		for(i=0;i<FONT_SIZE;i++)
		{
			tmp = pgm_read_byte(&tmp_ptr[(Ascii-'0')*FONT_SIZE+i]);
			for(j=0;j<8;j++)
			{
				if(cnv) GLCD_Pixel(col+j,row+i,~(tmp>>j)&0x01);
				else GLCD_Pixel(col+j,row+i,(tmp>>j)&0x01);
			}
		}

	}
	if(Ascii>='a' && Ascii<='z')
	{
		tmp_ptr = nFontSmAlp;
		for(i=0;i<FONT_SIZE;i++)
		{
			tmp = pgm_read_byte(&tmp_ptr[(Ascii-'a')*FONT_SIZE+i]);
			for(j=0;j<8;j++)
			{
				if(cnv) GLCD_Pixel(col+j,row+i,~(tmp>>j)&0x01);
				else GLCD_Pixel(col+j,row+i,(tmp>>j)&0x01);
			}
		}

	}

}
void GLCD_DispString(INT16U col,INT16U row,INT8U* str,INT8U cnv)
{
	INT8U nLength,i;
	nLength = strlen(str);
	for(i=0;i<nLength;i++)
		GLCD_DispFont(col+8*i,row,*(str+i),cnv);
}
// glcd functions end
void main()
{
	char strBuff[20]={0};
	char count;
	initPort();
	// lcd init
	#if 1
	GLCD_Init_1();
	GLCD_Clear();
	GLCD_DispImag(2,0);//LCD_IMG_CNV);
	#endif
	// end lcd init
	usart1_init();
	can_init(b250K);
	//can_rx_set(1,0x23,EXT,8,0x1fffffff,0x05);//
	can_int_rx_set(1,0x23,EXT,8,0x1fffffff,0x05);
	count =0;
	flg_can=0;
	msg1.rtr=0;
	initRcServoMotor();
	CtrlRcServoMotor(0,90);
	CtrlRcServoMotor(1,90);
	CtrlRcServoMotor(2,90);
	sprintf(strBuff,"Start FW Can Rx id(%d) Tx id(%d)",0x23,msg1.id);
	usart1_transmit_string(strBuff);
	sei();
	while(1)
	{

//		if((PORTA & (1<<6))) PORTA&=~(1<<6);
//		else PORTA |= (1<<6);
		//can_rx(1,&msg1);
		if(flg_can)
		{
			sprintf(strBuff,"\rTxID=%4ld\n",msg1.id);
			usart1_transmit_string(strBuff);
			sprintf(strBuff,"\rU1=%4d,U2=%4d\n",((0xff00&(msg1.data[0]<<8)) | (0xff&msg1.data[1])),((0xff00&(msg1.data[2]<<8)) | (0xff&msg1.data[3])));
			usart1_transmit_string(strBuff);
			sprintf(strBuff,"\rAdc1=%4d,Adc2=%4d\n",((0xff00&(msg1.data[4]<<8)) | (0xff&msg1.data[5])),((0xff00&(msg1.data[6]<<8)) | (0xff&msg1.data[7])));
			usart1_transmit_string(strBuff);
			//_delay_ms(200);
			usart1_transmit_string("\rID-");
			usart1_transmit(msg1.id+0x30);
			usart1_transmit_string(", format-");
			usart1_transmit(msg1.ide+0x30);
			usart1_transmit_string(", DLC-");
			usart1_transmit(msg1.dlc+0x30);
			//usart1_transmit_string(" Data");
			//usart1_transmit_string(msg1.data);
			usart1_transmit_string(", End\r\n");
			flg_can=0;
			if(PORTA & (1<<6)) PORTA &= (~(1<<6));
			else PORTA|=(1<<6);
		}
		else
		_delay_ms(10);
		can_tx(1,&msg2);

	}
}
