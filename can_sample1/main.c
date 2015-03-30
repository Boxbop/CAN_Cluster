#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "myCANLib.h"
#include "usartLib.h"
#include "ultraSonic.h"
#include "myDelay.h"

//
#define FREQ_CLKIO 		16000000
#define PRESCAIL 		64
#define RC_SERVO_PERIOD	25
//
struct MOb msg1 = {	0x023,
					0,
					EXT,
					8,
					{0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38}};

void initPort()
{
	DDRB = 0xf1;
	DDRC = 0xff;
	PORTC = 0x7f;
	DDRD = 0x3b;
	DDRG = 0xff;
	PORTG = 0x00;
}

void initAdc(void)
{
	ADMUX = 0x40;
	ADCSRA = 0xc8;
	DDRF = 0x00;
	DIDR0 = 0xff;
}

unsigned int ADdata;
SIGNAL(ADC_vect)
{
	ADdata=ADC;
	ADCSRA = 0xc8;
}

unsigned int read_Adc(char id)
{
	ADMUX = (0x40 | id);
//	ms_delay(1);
//	ADC=0;
	//ADCSRA = 0xc8;
	ms_delay(1);
	return ADdata;
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

void main()
{
	char strBuff[20]={0};
	char count;
	unsigned int distance,adc_data;
	initPort();
	usart1_init();
	can_init(b250K);
	init_EXT_INT(0x0f);
	init_TIMER0_COMPA();
	initAdc();
	count =0;
	distance=0;
	msg1.rtr=0;
	initRcServoMotor();
	CtrlRcServoMotor(0,45);
	CtrlRcServoMotor(1,90);
	CtrlRcServoMotor(2,135);
	ms_delay(200);
	sei();
	while(1)
	{
		if((PORTC & (1<<6))==1) PORTC&=~(1<<6);
		else PORTC |= (1<<6);
		count++;
		if(count==8) count =3;
		CtrlRcServoMotor(0,count*18);
		//CtrlRcServoMotor(1,count*18);
		//CtrlRcServoMotor(2,count*18);

		//msg1.data[4] = count+0x30;
		distance=0;
		distance = Ultra_Sonic1();
		sprintf(strBuff,"\rUltra_distance1=%d\n",distance);
		usart1_transmit_string(strBuff);
		ms_delay(50);
		msg1.data[0]=(distance & 0xff00)>>8;
		msg1.data[1]=distance & 0xff;
		distance=0;
		distance = Ultra_Sonic2();
		sprintf(strBuff,"\rUltra_distance2=%d\n",distance);
		usart1_transmit_string(strBuff);
		ms_delay(50);
		msg1.data[2]=(distance & 0xff00)>>8;
		msg1.data[3]=distance & 0xff;
		adc_data=read_Adc(5);
		ms_delay(50);
		msg1.data[4]=(adc_data & 0xff00)>>8;
		msg1.data[5]=distance & 0xff;
		adc_data=read_Adc(6);
		ms_delay(50);
		msg1.data[6]=(adc_data & 0xff00)>>8;
		msg1.data[7]=distance & 0xff;
		can_tx(7,&msg1);//,msg1.rtr);
		sprintf(strBuff,"\rTxData=%d\n",msg1.data[4]);
		usart1_transmit_string(strBuff);
		sprintf(strBuff,"\rIr1=%d\n",adc_data);
		usart1_transmit_string(strBuff);
		ms_delay(50);
		adc_data=read_Adc(6);
		sprintf(strBuff,"\rIr2=%d\n",adc_data);
		usart1_transmit_string(strBuff);
		ms_delay(100);
	}
}
