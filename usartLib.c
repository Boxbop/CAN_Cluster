#include <avr/io.h>
#include "usartLib.h"

//
#define CPU_CLOCK_HZ	16000000
#define BAUDRATE0 2000
//USART 1 =======================================USART 1
void usart1_init(void)
{
	UCSR1A=0x00;
	UCSR1B=0x98;
	UCSR1C=0x06;
	UBRR1H=0x00;
	UBRR1L=CPU_CLOCK_HZ/BAUDRATE0/16-1;
}

/* Read and write functions */
unsigned char usart1_receive( void )
{
	/* Wait for incomming data */
	while ( !(UCSR1A & (1<<RXC1)) );			                
	/* Return the data */
	return UDR1;
}

void usart1_transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !(UCSR1A & (1<<UDRE1)) ); 			                
	/* Start transmittion */
	UDR1 = data; 			        
}

void usart1_transmit_string(char* p){
	while (*p)
		usart1_transmit(*p++);
}

//USART 0 =======================================USART 0
void usart0_init(void)
{
	UCSR0A=0x00;
	UCSR0B=0x98;
	UCSR0C=0x06;
	UBRR0H=0x00;
	UBRR0L=CPU_CLOCK_HZ/BAUDRATE0/16-1;			// 9600 bps
}


/* Read and write functions */
unsigned char usart0_receive( void )
{
	/* Wait for incomming data */
	while ( !(UCSR0A & (1<<RXC0)) );			                
	/* Return the data */
	return UDR0;
}

void usart0_transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !(UCSR0A & (1<<UDRE0)) ); 			                
	/* Start transmittion */
	UDR0 = data; 			        
}

void usart0_transmit_string(char* p){
	while (*p)
		usart0_transmit(*p++);
}
