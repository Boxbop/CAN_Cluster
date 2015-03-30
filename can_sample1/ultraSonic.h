#include <avr/io.h>

// 초음파센서 제어를 위한 정의 ================================
#define PE4_Clear	(PORTE &= 0xef)
#define PE4_Set		(PORTE |= 0x10)
#define PE4_IN		(DDRE &= 0xef)
#define PE4_OUT		(DDRE |= 0x10)
//
#define PE5_Clear	(PORTE &= 0xdf)
#define PE5_Set		(PORTE |= 0x20)
#define PE5_IN		(DDRE &= 0xdf)
#define PE5_OUT		(DDRE |= 0x20)
//
#define PE6_Clear	(PORTE &= 0xbf)
#define PE6_Set		(PORTE |= 0x40)
#define PE6_IN		(DDRE &= 0xbf)
#define PE6_OUT		(DDRE |= 0x40)
//
#define PE7_Clear	(PORTE &= 0x7f)
#define PE7_Set		(PORTE |= 0x80)
#define PE7_IN		(DDRE &= 0x7f)
#define PE7_OUT		(DDRE |= 0x80)

#define TEMPERATURE 25	// 거리 측정을 위한 온도 설정 

void init_EXT_INT(unsigned char id);
void init_TIMER0_COMPA(void);
unsigned int Ultra_Sonic1(void);
unsigned int Ultra_Sonic2(void);
unsigned int Ultra_Sonic3(void);
unsigned int Ultra_Sonic4(void);
