#include <avr/io.h>
#include <avr/interrupt.h>

#include "ultraSonic.h"
//#include "myDelay.h"
#include <avr/delay.h>
//Ultra Sonic====================================================
unsigned short tick=0, pulse_check=0, pulse_end=0;

//=========================================================================
//초음파센서 거리 측정을 위한 외부 인터럽트 초기화 
void init_EXT_INT(unsigned char id)
{
	if(id & 0x01)
	{
		EIMSK |= (1<<INT4);					// INT4 외부 인터럽트 설정 
		EICRB |= (1<<ISC41)|(1<<ISC40); 	// INT4 상승에지 인터럽트 설정 
		EIFR   = 1<<INT4;					// INT4 외부 인터럽트 플래그 설정 
	}
	if(id & 0x02)
	{
		EIMSK |= (1<<INT5);					// INT5 외부 인터럽트 설정 
		EICRB |= (1<<ISC51)|(1<<ISC50); 	// INT5 상승에지 인터럽트 설정 
		EIFR   = 1<<INT5;					// INT5 외부 인터럽트 플래그 설정 
	}
	if(id & 0x04)
	{
		EIMSK |= (1<<INT6);					// INT6 외부 인터럽트 설정 
		EICRB |= (1<<ISC61)|(1<<ISC60); 	// INT6 상승에지 인터럽트 설정 
		EIFR   = 1<<INT6;					// INT6 외부 인터럽트 플래그 설정 
	}
	if(id & 0x08)
	{
		EIMSK |= (1<<INT7);					// INT7 외부 인터럽트 설정 
		EICRB |= (1<<ISC71)|(1<<ISC70); 	// INT7 상승에지 인터럽트 설정 
		EIFR   = 1<<INT7;					// INT7 외부 인터럽트 플래그 설정 
	}
}

//===========================================================================
// 타이머/카운터 0 초기화
void init_TIMER0_COMPA(void)
{
	TCCR0A |= (1<<WGM01);		// 타미어/카운터 0; CTC 모드 설정 
	TCCR0A |= (1<<CS01);		// 1/8 프리스케일; 16MHz/8= 2MHz, 
								// 주기; 1/20 sec 
	TCNT0 = 0;					// 타이머/카운터 0 초기화 
	OCR0A = 19;					// 1주기; 20used = 1/(16Mhz/(2*8*(19+1)))
	//OCR0A = 9;					// 1주기; 10used = 1/(16Mhz/(2*8*(9+1)))
								// CTC 인터럽트 주기ㅣ; 10usec					
	TIMSK0 = (1<<OCIE0A);		// 타이머카운터 출력 비교 일치 인터럽트 인에이블 
								// Output Compare Match Interrup Enable
	TIFR0 = 0x00;				// 플래그 레지스터 클리어 
}

SIGNAL(TIMER0_COMP_vect)	
{
	tick++;						// 10usec 마다  업 카운트 
}

SIGNAL(INT4_vect)
{
	unsigned short pulse_tick;

	pulse_tick = tick;

	if(EICRB & 0x03)		// 상승에지 인가?
	{
	   EICRB &= 0x00;		// INT4 low 상태 인터럽트 설정  
	   tick = 0;			// tick 값 클리어 
	}
	else
	{
		EICRB |= (1<<ISC41)|(1<<ISC40);	// INT4 상승 에지 인터럽트 설정 
		pulse_end = pulse_tick;	// 에코 펄스 카운트 
	}
}
//
SIGNAL(INT5_vect)
{
	unsigned short pulse_tick;

	pulse_tick = tick;

	if(EICRB & (0x03<<2))		// 상승에지 인가?
	{
	   EICRB &= 0x00;		// INT4 low 상태 인터럽트 설정  
	   tick = 0;			// tick 값 클리어 
	}
	else
	{
		EICRB |= (1<<ISC51)|(1<<ISC50);	// INT4 상승 에지 인터럽트 설정 
		pulse_end = pulse_tick;	// 에코 펄스 카운트 
	}
}
//
SIGNAL(INT6_vect)
{
	unsigned short pulse_tick;

	pulse_tick = tick;

	if(EICRB & (0x03<<4))		// 상승에지 인가?
	{
	   EICRB &= 0x00;		// INT4 low 상태 인터럽트 설정  
	   tick = 0;			// tick 값 클리어 
	}
	else
	{
		EICRB |= (1<<ISC61)|(1<<ISC60);	// INT4 상승 에지 인터럽트 설정 
		pulse_end = pulse_tick;	// 에코 펄스 카운트 
	}
}
//
SIGNAL(INT7_vect)
{
	unsigned short pulse_tick;

	pulse_tick = tick;

	if(EICRB & (0x03<<6))		// 상승에지 인가?
	{
	   EICRB &= 0x00;		// INT4 low 상태 인터럽트 설정  
	   tick = 0;			// tick 값 클리어 
	}
	else
	{
		EICRB |= (1<<ISC71)|(1<<ISC70);	// INT4 상승 에지 인터럽트 설정 
		pulse_end = pulse_tick;	// 에코 펄스 카운트 
	}
}
//
unsigned int Ultra_Sonic1(void)
{
	unsigned int distance;
		
	cli();				// 인터럽트 클리어 
	PE4_Clear;
	PE4_OUT;			// PportE4 핀 출력 설정 
	//us_delay(500);
	_delay_us(500);

	PE4_Set;			// PortE4 핀 '1' 출력 
	//us_delay(5);		// 5usec 유지 
	_delay_us(5);

	PE4_Clear;		    // PortE4 핀 '0' 출력 
	//us_delay(100);
	_delay_us(100);

	PE4_IN;				// PortE4 핀 입력 설정 
	//us_delay(100);
	_delay_us(100);

	sei();				// 모든 인터럽트 인에이블 
	//ms_delay(10);
	_delay_ms(10);
	/* distance = velocity * time */
	distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;
								 // pulse; 1 펄스당 10usec
								 // *1/2; 왕복
								 // *1000; 거리 mm 단위 
	return distance;
}
//
unsigned int Ultra_Sonic2(void)
{
	unsigned int distance;
		
	cli();				// 인터럽트 클리어 
	PE5_Clear;
	PE5_OUT;			// PportE4 핀 출력 설정 
	//us_delay(500);
	_delay_us(500);

	PE5_Set;			// PortE4 핀 '1' 출력 
	//us_delay(5);		// 5usec 유지 
	_delay_us(5);

	PE5_Clear;		    // PortE4 핀 '0' 출력 
	//us_delay(100);
	_delay_us(100);

	PE5_IN;				// PortE4 핀 입력 설정 
	//us_delay(100);
	_delay_us(100);

	sei();				// 모든 인터럽트 인에이블 
	//ms_delay(10);
	_delay_ms(10);

	/* distance = velocity * time */
	distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;
								 // pulse; 1 펄스당 10usec
								 // *1/2; 왕복
								 // *1000; 거리 mm 단위 
	return distance;
}
//
unsigned int Ultra_Sonic3(void)
{
	unsigned int distance;
		
	cli();				// 인터럽트 클리어 
	PE6_Clear;
	PE6_OUT;			// PportE4 핀 출력 설정 
	//us_delay(500);
	_delay_us(500);

	PE6_Set;			// PortE4 핀 '1' 출력 
	//us_delay(5);		// 5usec 유지 
	_delay_us(5);

	PE6_Clear;		    // PortE4 핀 '0' 출력 
	//us_delay(100);
	_delay_us(100);

	PE6_IN;				// PortE4 핀 입력 설정 
	//us_delay(100);
	_delay_us(100);

	sei();				// 모든 인터럽트 인에이블 
	//ms_delay(10);
	_delay_ms(10);

	/* distance = velocity * time */
	distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;
								 // pulse; 1 펄스당 10usec
								 // *1/2; 왕복
								 // *1000; 거리 mm 단위 
	return distance;
}
//
unsigned int Ultra_Sonic4(void)
{
	unsigned int distance;
		
	cli();				// 인터럽트 클리어 
	PE7_Clear;
	PE7_OUT;			// PportE4 핀 출력 설정 
	//us_delay(500);
	_delay_us(500);

	PE7_Set;			// PortE4 핀 '1' 출력 
	//us_delay(5);		// 5usec 유지 
	_delay_us(5);

	PE7_Clear;		    // PortE4 핀 '0' 출력 
	//us_delay(100);
	_delay_us(100);

	PE7_IN;				// PortE4 핀 입력 설정 
	//us_delay(100);
	_delay_us(100);

	sei();				// 모든 인터럽트 인에이블 
	//ms_delay(10);
	_delay_ms(10);
	/* distance = velocity * time */
	distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;
								 // pulse; 1 펄스당 10usec
								 // *1/2; 왕복
								 // *1000; 거리 mm 단위 
	return distance;
}
