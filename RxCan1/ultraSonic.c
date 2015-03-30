#include <avr/io.h>
#include <avr/interrupt.h>

#include "ultraSonic.h"
//#include "myDelay.h"
#include <avr/delay.h>
//Ultra Sonic====================================================
unsigned short tick=0, pulse_check=0, pulse_end=0;

//=========================================================================
//�����ļ��� �Ÿ� ������ ���� �ܺ� ���ͷ�Ʈ �ʱ�ȭ 
void init_EXT_INT(unsigned char id)
{
	if(id & 0x01)
	{
		EIMSK |= (1<<INT4);					// INT4 �ܺ� ���ͷ�Ʈ ���� 
		EICRB |= (1<<ISC41)|(1<<ISC40); 	// INT4 ��¿��� ���ͷ�Ʈ ���� 
		EIFR   = 1<<INT4;					// INT4 �ܺ� ���ͷ�Ʈ �÷��� ���� 
	}
	if(id & 0x02)
	{
		EIMSK |= (1<<INT5);					// INT5 �ܺ� ���ͷ�Ʈ ���� 
		EICRB |= (1<<ISC51)|(1<<ISC50); 	// INT5 ��¿��� ���ͷ�Ʈ ���� 
		EIFR   = 1<<INT5;					// INT5 �ܺ� ���ͷ�Ʈ �÷��� ���� 
	}
	if(id & 0x04)
	{
		EIMSK |= (1<<INT6);					// INT6 �ܺ� ���ͷ�Ʈ ���� 
		EICRB |= (1<<ISC61)|(1<<ISC60); 	// INT6 ��¿��� ���ͷ�Ʈ ���� 
		EIFR   = 1<<INT6;					// INT6 �ܺ� ���ͷ�Ʈ �÷��� ���� 
	}
	if(id & 0x08)
	{
		EIMSK |= (1<<INT7);					// INT7 �ܺ� ���ͷ�Ʈ ���� 
		EICRB |= (1<<ISC71)|(1<<ISC70); 	// INT7 ��¿��� ���ͷ�Ʈ ���� 
		EIFR   = 1<<INT7;					// INT7 �ܺ� ���ͷ�Ʈ �÷��� ���� 
	}
}

//===========================================================================
// Ÿ�̸�/ī���� 0 �ʱ�ȭ
void init_TIMER0_COMPA(void)
{
	TCCR0A |= (1<<WGM01);		// Ÿ�̾�/ī���� 0; CTC ��� ���� 
	TCCR0A |= (1<<CS01);		// 1/8 ����������; 16MHz/8= 2MHz, 
								// �ֱ�; 1/20 sec 
	TCNT0 = 0;					// Ÿ�̸�/ī���� 0 �ʱ�ȭ 
	OCR0A = 19;					// 1�ֱ�; 20used = 1/(16Mhz/(2*8*(19+1)))
	//OCR0A = 9;					// 1�ֱ�; 10used = 1/(16Mhz/(2*8*(9+1)))
								// CTC ���ͷ�Ʈ �ֱ��; 10usec					
	TIMSK0 = (1<<OCIE0A);		// Ÿ�̸�ī���� ��� �� ��ġ ���ͷ�Ʈ �ο��̺� 
								// Output Compare Match Interrup Enable
	TIFR0 = 0x00;				// �÷��� �������� Ŭ���� 
}

SIGNAL(TIMER0_COMP_vect)	
{
	tick++;						// 10usec ����  �� ī��Ʈ 
}

SIGNAL(INT4_vect)
{
	unsigned short pulse_tick;

	pulse_tick = tick;

	if(EICRB & 0x03)		// ��¿��� �ΰ�?
	{
	   EICRB &= 0x00;		// INT4 low ���� ���ͷ�Ʈ ����  
	   tick = 0;			// tick �� Ŭ���� 
	}
	else
	{
		EICRB |= (1<<ISC41)|(1<<ISC40);	// INT4 ��� ���� ���ͷ�Ʈ ���� 
		pulse_end = pulse_tick;	// ���� �޽� ī��Ʈ 
	}
}
//
SIGNAL(INT5_vect)
{
	unsigned short pulse_tick;

	pulse_tick = tick;

	if(EICRB & (0x03<<2))		// ��¿��� �ΰ�?
	{
	   EICRB &= 0x00;		// INT4 low ���� ���ͷ�Ʈ ����  
	   tick = 0;			// tick �� Ŭ���� 
	}
	else
	{
		EICRB |= (1<<ISC51)|(1<<ISC50);	// INT4 ��� ���� ���ͷ�Ʈ ���� 
		pulse_end = pulse_tick;	// ���� �޽� ī��Ʈ 
	}
}
//
SIGNAL(INT6_vect)
{
	unsigned short pulse_tick;

	pulse_tick = tick;

	if(EICRB & (0x03<<4))		// ��¿��� �ΰ�?
	{
	   EICRB &= 0x00;		// INT4 low ���� ���ͷ�Ʈ ����  
	   tick = 0;			// tick �� Ŭ���� 
	}
	else
	{
		EICRB |= (1<<ISC61)|(1<<ISC60);	// INT4 ��� ���� ���ͷ�Ʈ ���� 
		pulse_end = pulse_tick;	// ���� �޽� ī��Ʈ 
	}
}
//
SIGNAL(INT7_vect)
{
	unsigned short pulse_tick;

	pulse_tick = tick;

	if(EICRB & (0x03<<6))		// ��¿��� �ΰ�?
	{
	   EICRB &= 0x00;		// INT4 low ���� ���ͷ�Ʈ ����  
	   tick = 0;			// tick �� Ŭ���� 
	}
	else
	{
		EICRB |= (1<<ISC71)|(1<<ISC70);	// INT4 ��� ���� ���ͷ�Ʈ ���� 
		pulse_end = pulse_tick;	// ���� �޽� ī��Ʈ 
	}
}
//
unsigned int Ultra_Sonic1(void)
{
	unsigned int distance;
		
	cli();				// ���ͷ�Ʈ Ŭ���� 
	PE4_Clear;
	PE4_OUT;			// PportE4 �� ��� ���� 
	//us_delay(500);
	_delay_us(500);

	PE4_Set;			// PortE4 �� '1' ��� 
	//us_delay(5);		// 5usec ���� 
	_delay_us(5);

	PE4_Clear;		    // PortE4 �� '0' ��� 
	//us_delay(100);
	_delay_us(100);

	PE4_IN;				// PortE4 �� �Է� ���� 
	//us_delay(100);
	_delay_us(100);

	sei();				// ��� ���ͷ�Ʈ �ο��̺� 
	//ms_delay(10);
	_delay_ms(10);
	/* distance = velocity * time */
	distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;
								 // pulse; 1 �޽��� 10usec
								 // *1/2; �պ�
								 // *1000; �Ÿ� mm ���� 
	return distance;
}
//
unsigned int Ultra_Sonic2(void)
{
	unsigned int distance;
		
	cli();				// ���ͷ�Ʈ Ŭ���� 
	PE5_Clear;
	PE5_OUT;			// PportE4 �� ��� ���� 
	//us_delay(500);
	_delay_us(500);

	PE5_Set;			// PortE4 �� '1' ��� 
	//us_delay(5);		// 5usec ���� 
	_delay_us(5);

	PE5_Clear;		    // PortE4 �� '0' ��� 
	//us_delay(100);
	_delay_us(100);

	PE5_IN;				// PortE4 �� �Է� ���� 
	//us_delay(100);
	_delay_us(100);

	sei();				// ��� ���ͷ�Ʈ �ο��̺� 
	//ms_delay(10);
	_delay_ms(10);

	/* distance = velocity * time */
	distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;
								 // pulse; 1 �޽��� 10usec
								 // *1/2; �պ�
								 // *1000; �Ÿ� mm ���� 
	return distance;
}
//
unsigned int Ultra_Sonic3(void)
{
	unsigned int distance;
		
	cli();				// ���ͷ�Ʈ Ŭ���� 
	PE6_Clear;
	PE6_OUT;			// PportE4 �� ��� ���� 
	//us_delay(500);
	_delay_us(500);

	PE6_Set;			// PortE4 �� '1' ��� 
	//us_delay(5);		// 5usec ���� 
	_delay_us(5);

	PE6_Clear;		    // PortE4 �� '0' ��� 
	//us_delay(100);
	_delay_us(100);

	PE6_IN;				// PortE4 �� �Է� ���� 
	//us_delay(100);
	_delay_us(100);

	sei();				// ��� ���ͷ�Ʈ �ο��̺� 
	//ms_delay(10);
	_delay_ms(10);

	/* distance = velocity * time */
	distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;
								 // pulse; 1 �޽��� 10usec
								 // *1/2; �պ�
								 // *1000; �Ÿ� mm ���� 
	return distance;
}
//
unsigned int Ultra_Sonic4(void)
{
	unsigned int distance;
		
	cli();				// ���ͷ�Ʈ Ŭ���� 
	PE7_Clear;
	PE7_OUT;			// PportE4 �� ��� ���� 
	//us_delay(500);
	_delay_us(500);

	PE7_Set;			// PortE4 �� '1' ��� 
	//us_delay(5);		// 5usec ���� 
	_delay_us(5);

	PE7_Clear;		    // PortE4 �� '0' ��� 
	//us_delay(100);
	_delay_us(100);

	PE7_IN;				// PortE4 �� �Է� ���� 
	//us_delay(100);
	_delay_us(100);

	sei();				// ��� ���ͷ�Ʈ �ο��̺� 
	//ms_delay(10);
	_delay_ms(10);
	/* distance = velocity * time */
	distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;
								 // pulse; 1 �޽��� 10usec
								 // *1/2; �պ�
								 // *1000; �Ÿ� mm ���� 
	return distance;
}
