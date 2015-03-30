#include <avr/io.h>
#include "usartLib.h"

//USART 1 =======================================USART 1
void usart1_init(void)
{
#if 1
 	UBRR1L=16000000/9600/16-1;                     /* Set the baud rate                            */
	UCSR1B = _BV(TXEN1)|_BV(RXEN1)| _BV(RXCIE1) ;//rx interrupt ,tx enable, rx enable
  	UCSR1C=_BV(UCSZ11)|_BV(UCSZ10);                       /* 8-bit data                                       */
#else
	UCSR1A=0x00;
	UCSR1B=0x98;
	UCSR1C=0x06;
	UBRR1H=0x00;
	UBRR1L=103;
#endif
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
	UBRR0L=103;			// 9600 bps
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
