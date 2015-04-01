/*
차량용 CAN 통신 계기판 제어 코드 Test
Target : YF sonata (LPI)
*/



#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "mycanlib.h"

//Custom option (AVR-GCC, MAKE)
//C:\WinAVR-20100110\bin\avr-gcc.exe
//C:\WinAVR-20100110\utils\bin\make.exe



//Hyundai YF Sonata
/*
struct MOb msg_YFS0 = { 0x43F,0,STD,8,{0x11,0x45,0x50,0xFF,0x82,0x00,0x00,0x00}};
struct MOb msg_YFS1 = { 0x440,0,STD,8,{0xFF,0x00,0x00,0x00,0xFF,0x00,0x00,0x00}};
struct MOb msg_YFS2 = { 0x18F,0,STD,8,{0xFA,0x4C,0x00,0x00,0x00,0x00,0x00,0x00}};
struct MOb msg_YFS3 = { 0x0A0,0,STD,8,{0x61,0x83,0xEC,0x09,0x00,0x1F,0x04,0x00}};
struct MOb msg_YFS4 = { 0x0A1,0,STD,8,{0x80,0x7B,0x00,0x00,0x26,0x00,0x00,0x00}};
struct MOb msg_YFS5 = { 0x260,0,STD,8,{0x20,0x2A,0x29,0xB0,0x0C,0x8E,0x75,0x00}};
struct MOb msg_YFS6 = { 0x271,0,STD,8,{0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00}};
struct MOb msg_YFS7 = { 0x2A0,0,STD,8,{0x00,0x00,0x9D,0x1C,0x15,0x06,0xEF,0x01}};
struct MOb msg_YFS8 = { 0x316,0,STD,8,{0x01,0x2A,0xEC,0x09,0x2A,0x15,0x00,0x70}};
struct MOb msg_YFS9 = { 0x329,0,STD,8,{0xDC,0xBA,0x81,0x04,0x06,0x20,0x00,0x14}};
struct MOb msg_YFS10 = { 0x370,0,STD,8,{0xFF,0x3F,0x00,0x01,0x00,0x00,0x00,0x00}};
struct MOb msg_YFS11 = { 0x545,0,STD,8,{0xE6,0x00,0x00,0x8A,0x00,0x00,0x00,0x00}};
struct MOb msg_YFS12 = { 0x2C0,0,STD,8,{0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
struct MOb msg_YFS13 = { 0x350,0,STD,8,{0x05,0x28,0xF4,0x7F,0x83,0x00,0x00,0x25}};
struct MOb msg_YFS14 = { 0x316,0,STD,8,{0x01,0x2B,0xEC,0x09,0x2B,0x15,0x00,0x70}};
*/
struct MOb msg_YFS[15] = {
{ 0x43F,0,STD,8,{0x11,0x45,0x50,0xFF,0x82,0x00,0x00,0x00}},
{ 0x440,0,STD,8,{0xFF,0x00,0x00,0x00,0xFF,0x00,0x00,0x00}},
{ 0x18F,0,STD,8,{0xFA,0x4C,0x00,0x00,0x00,0x00,0x00,0x00}},
{ 0x0A0,0,STD,8,{0x61,0x83,0xEC,0x09,0x00,0x1F,0x04,0x00}},
{ 0x0A1,0,STD,8,{0x80,0x7B,0x00,0x00,0x26,0x00,0x00,0x00}},
{ 0x260,0,STD,8,{0x20,0x2A,0x29,0xB0,0x0C,0x8E,0x75,0x00}},
{ 0x271,0,STD,8,{0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00}},
{ 0x2A0,0,STD,8,{0x00,0x00,0x9D,0x1C,0x15,0x06,0xEF,0x01}},
{ 0x316,0,STD,8,{0x01,0x2A,0xEC,0x09,0x2A,0x15,0x00,0x70}},
{ 0x329,0,STD,8,{0xDC,0xBA,0x81,0x04,0x06,0x20,0x00,0x14}},
{ 0x370,0,STD,8,{0xFF,0x3F,0x00,0x01,0x00,0x00,0x00,0x00}},
{ 0x545,0,STD,8,{0xE6,0x00,0x00,0x8A,0x00,0x00,0x00,0x00}},
{ 0x2C0,0,STD,8,{0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{ 0x350,0,STD,8,{0x05,0x28,0xF4,0x7F,0x83,0x00,0x00,0x25}},
{ 0x316,0,STD,8,{0x01,0x2B,0xEC,0x09,0x2B,0x15,0x00,0x70}}					
};

//Hyundai unknown
/*
struct MOb msg_HDU0 = { 0x43F,0,STD,8,{0x11,0x4F,0x60,0xFF,0x00,0x00,0x00,0x00}};
struct MOb msg_HDU1 = { 0x43F,0,STD,8,{0x15,0x47,0x60,0xFF,0x76,0x00,0x00,0x00}};
struct MOb msg_HDU2 = { 0x43F,0,STD,8,{0x08,0x46,0x60,0xFF,0x00,0x00,0x00,0x00}};
struct MOb msg_HDU3 = { 0x0A0,0,STD,8,{0x00,0x00,0x00,0x00,0x00,0xFF,0x01,0x00}};
struct MOb msg_HDU4 = { 0x0A1,0,STD,8,{0x80,0x80,0x00,0x00,0x09,0x00,0x00,0x00}};
struct MOb msg_HDU5 = { 0x316,0,STD,8,{0x05,0x00,0x00,0x00,0x00,0x2C,0x00,0x80}};
struct MOb msg_HDU6 = { 0x316,0,STD,8,{0x85,0x00,0x00,0x00,0x00,0x4E,0x00,0x80}};
struct MOb msg_HDU7 = { 0x545,0,STD,8,{0xDE,0x00,0x00,0x85,0x00,0x00,0x00,0x00}};
*/
struct MOb msg_HDU[8] = {
{ 0x43F,0,STD,8,{0x11,0x4F,0x60,0xFF,0x00,0x00,0x00,0x00}},
{ 0x43F,0,STD,8,{0x15,0x47,0x60,0xFF,0x76,0x00,0x00,0x00}},
{ 0x43F,0,STD,8,{0x08,0x46,0x60,0xFF,0x00,0x00,0x00,0x00}},
{ 0x0A0,0,STD,8,{0x00,0x00,0x00,0x00,0x00,0xFF,0x01,0x00}},
{ 0x0A1,0,STD,8,{0x80,0x80,0x00,0x00,0x09,0x00,0x00,0x00}},
{ 0x316,0,STD,8,{0x05,0x00,0x00,0x00,0x00,0x2C,0x00,0x80}},
{ 0x316,0,STD,8,{0x85,0x00,0x00,0x00,0x00,0x4E,0x00,0x80}},
{ 0x545,0,STD,8,{0xDE,0x00,0x00,0x85,0x00,0x00,0x00,0x00}}
};

//Hyundai New Carens
/*
struct MOb msg_NCR0 = { 0x271,0,STD,8,{0x47,0x0D,0xC0,0x08,0x98,0x57,0x00,0x03}};
struct MOb msg_NCR1 = { 0x440,0,STD,8,{0x00,0x00,0x00,0x04,0xFF,0x2A,0x0B,0x00}};
struct MOb msg_NCR2 = { 0x43F,0,STD,8,{0x00,0x40,0x40,0xFF,0x31,0x24,0x0B,0x00}};
struct MOb msg_NCR3 = { 0x316,0,STD,8,{0x01,0x23,0x9C,0x0C,0x23,0x1C,0x00,0x7E}};
struct MOb msg_NCR4 = { 0x280,0,STD,8,{0x21,0x0D,0x84,0x95,0x19,0x1B,0x4B,0x2E}};
struct MOb msg_NCR5 = { 0x2A0,0,STD,8,{0x00,0x00,0x4B,0x1C,0xCE,0x0D,0x90,0x08}};
struct MOb msg_NCR6 = { 0x329,0,STD,8,{0x86,0x95,0x7D,0x04,0x04,0x20,0xFF,0x14}};
struct MOb msg_NCR7 = { 0x545,0,STD,8,{0xD0,0xD4,0x00,0x8F,0x00,0x00,0x00,0x00}};
*/
struct MOb msg_NCR[8] = {
{ 0x271,0,STD,8,{0x47,0x0D,0xC0,0x08,0x98,0x57,0x00,0x03} },

//0x440,0,STD,8,{0x00,0x00,0x00,0x04,0xFF,0x2A,0x0B,0x00}
{ 0x440,0,STD,8,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00} },

//0x43F,0,STD,8,{0x00,0x40,0x40,0xFF,0x31,0x24,0x0B,0x00} 2번째 기어조절 (Default-> P단기어, 0x55->D단 변경, 0x56->N단 변경, 0x57->R단 변경)
{ 0x43F,0,STD,8,{0x00,0x57,0x00,0x00,0x00,0x00,0x00,0x00} }, 


{ 0x316,0,STD,8,{0x01,0x23,0x9C,0x0C,0x23,0x1C,0x00,0x7E} },

//0x280,0,STD,8,{0x21,0x0D,0x84,0x95,0x19,0x1B,0x4B,0x2E} 3번째 조절(속도 변화)
{ 0x280,0,STD,8,{0x21,0x0D,0x84,0x95,0x19,0x1B,0x4B,0x2E} }, 


{ 0x2A0,0,STD,8,{0x00,0x00,0x4B,0x1C,0xCE,0x0D,0x90,0x08} },


{ 0x329,0,STD,8,{0x86,0x95,0x7D,0x04,0x04,0x20,0xFF,0x14} },


{ 0x545,0,STD,8,{0xD0,0xD4,0x00,0x8F,0x00,0x00,0x00,0x00} }
};

//Toyota Camry
//ThrottlePedal 2C1, Engine Speed 2C4, Fuel Gage 398, Current Gear 1D0
/*
struct MOb msg_TCR0 = { 0x2C1,0,STD,8,{0x08,0x01,0xA3,0x00,0xC5,0xCD,0x00,0x09}};
struct MOb msg_TCR1 = { 0x2C4,0,STD,8,{0x03,0x5A,0x00,0x23,0x08,0x80,0x71,0x47}};
struct MOb msg_TCR2 = { 0x2C4,0,STD,8,{0x03,0x58,0x00,0x23,0x08,0x80,0x71,0x45}};
*/
struct MOb msg_TCR[3] = {
{ 0x2C1,0,STD,8,{0x08,0x01,0xA3,0x00,0xC5,0xCD,0x00,0x09}},
{ 0x2C4,0,STD,8,{0x03,0x5A,0x00,0x23,0x08,0x80,0x71,0x47}},
{ 0x2C4,0,STD,8,{0x03,0x58,0x00,0x23,0x08,0x80,0x71,0x45}}
};

void initPort()
{
	DDRC = 0xff;
	PORTC = 0xff;
	DDRG = 0xff;
	PORTG = 0x00;
	DDRE = 0xff;
	PORTE = 0xff;
}

void testPoint(int i)
{
	if(i == 0)
		PORTE=0xff;
	else
		PORTE=0x00;
}

void breakPoint()
{
	while(1)
	{
		PORTE=0x00;
	}
}

void main()
{
	char count;
	initPort();
	can_init(2);//b500k
	count =0;


	//_delay_ms(1000);

	
	while(1)
	{

		//struct MOb msg_NCR0 = { 0x271,0,STD,8,{0x47,0x0D,0xC0,0x08,0x98,0x57,0x00,0x03}};
//		can_tx(7,&msg_NCR[0],0);
//		_delay_ms(10);

		//struct MOb msg_NCR1 = { 0x440,0,STD,8,{0x00,0x00,0x00,0x04,0xFF,0x2A,0x0B,0x00}};
		//조합 메시지 -> 속도 상승
//		can_tx(7,&msg_NCR[1],0);
//		_delay_ms(1);

		//struct MOb msg_NCR2 = { 0x43F,0,STD,8,{0x00,0x40,0x40,0xFF,0x31,0x24,0x0B,0x00}};
		//단일 메시지 (10ms) -> P단기어 활성
		can_tx(7,&msg_NCR[2],0);
//		_delay_ms(10);

		//struct MOb msg_NCR3 = { 0x316,0,STD,8,{0x01,0x23,0x9C,0x0C,0x23,0x1C,0x00,0x7E}};
		//단일 메시지 (10ms) -> RPM 상승
//		can_tx(7,&msg_NCR[3],0);
//		_delay_ms(10);

		//struct MOb msg_NCR4 = { 0x280,0,STD,8,{0x21,0x0D,0x84,0x95,0x19,0x1B,0x4B,0x2E}};
		//3번째 데이터 바이트 조절 -> 속도 값 조절 가능 (조합메시지)
//		can_tx(7,&msg_NCR[4],0);
//		_delay_ms(1);

		//struct MOb msg_NCR5 = { 0x2A0,0,STD,8,{0x00,0x00,0x4B,0x1C,0xCE,0x0D,0x90,0x08}};
//		can_tx(7,&msg_NCR[5],0);
//		_delay_ms(10);

		//struct MOb msg_NCR6 = { 0x329,0,STD,8,{0x86,0x95,0x7D,0x04,0x04,0x20,0xFF,0x14}};
		//단일 메시지 (10ms) -> 수온 게이지 3칸 활성
//		can_tx(7,&msg_NCR[6],0);
//		_delay_ms(10);

		//struct MOb msg_NCR7 = { 0x545,0,STD,8,{0xD0,0xD4,0x00,0x8F,0x00,0x00,0x00,0x00}};
//		can_tx(7,&msg_NCR[7],0);
//		_delay_ms(10);


/*
		No delay 송신 -> 130km/h, 수온게이지 1칸 활성
		+struct MOb msg_NCR0 = { 0x271,0,STD,8,{0x47,0x0D,0xC0,0x08,0x98,0x57,0x00,0x03}}-> LPI 활성
		struct MOb msg_NCR1 = { 0x440,0,STD,8,{0x00,0x00,0x00,0x04,0xFF,0x2A,0x0B,0x00}}
		struct MOb msg_NCR4 = { 0x280,0,STD,8,{0x21,0x0D,0x84,0x95,0x19,0x1B,0x4B,0x2E}}
		struct MOb msg_NCR5 = { 0x2A0,0,STD,8,{0x00,0x00,0x4B,0x1C,0xCE,0x0D,0x90,0x08}}
		struct MOb msg_NCR6 = { 0x329,0,STD,8,{0x86,0x95,0x7D,0x04,0x04,0x20,0xFF,0x14}}


		No delay 송신 -> 200km/h 움직임 감지
		struct MOb msg_NCR0 = { 0x271,0,STD,8,{0x47,0x0D,0xC0,0x08,0x98,0x57,0x00,0x03}}
		struct MOb msg_NCR1 = { 0x440,0,STD,8,{0x00,0x00,0x00,0x04,0xFF,0x2A,0x0B,0x00}}
		struct MOb msg_NCR4 = { 0x280,0,STD,8,{0x21,0x0D,0x84,0x95,0x19,0x1B,0x4B,0x2E}}
		struct MOb msg_NCR5 = { 0x2A0,0,STD,8,{0x00,0x00,0x4B,0x1C,0xCE,0x0D,0x90,0x08}}

		No delay 송신 -> 140km/h (딜레이 걸어줄 경우 속도계 움직임)
		struct MOb msg_NCR1 = { 0x440,0,STD,8,{0x00,0x00,0x00,0x04,0xFF,0x2A,0x0B,0x00}}
		struct MOb msg_NCR4 = { 0x280,0,STD,8,{0x21,0x0D,0x84,0x95,0x19,0x1B,0x4B,0x2E}}

		No delay 송신 -> RPM 초반 미동, 160km/h
		struct MOb msg_NCR1 = { 0x440,0,STD,8,{0x00,0x00,0x00,0x04,0xFF,0x2A,0x0B,0x00}}
		struct MOb msg_NCR3 = { 0x316,0,STD,8,{0x01,0x23,0x9C,0x0C,0x23,0x1C,0x00,0x7E}}


*/

	}
}
	



