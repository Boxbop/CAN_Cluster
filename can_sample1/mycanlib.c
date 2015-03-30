#include <avr/io.h>
#include <avr/interrupt.h>

#include "myCANLib.h"
//#include "lcdControl.h"

//***************************************************************
// CAN 초기화 	
// 1. CAN 제어기 리셋 
// 2. 보레이트 설정 
// 3. MOb 모드/상태 초기화 
// 4. MOb IDT, Mask 초기화 
// 5. CAN General 인터럽트 클리어(CAN 인터럽트 사용하지 않음) 
// 6. CAN MOb 인터럽트 클리어(CAN MOb 인터럽트 사용하지 않음)
// 7. CAN 제어기 인에이블 모드 설정 
// 8. CAN 제어기 동작 확인 후 초기화 완료  									
//***************************************************************
void can_init (char baudRate)	// CAN초기화 
{
	unsigned char i, j;
	
	CANGCON |= (1<<SWRES);	// CAN 제어기 리셋
							// CAN General Control Register
  							
	//보레이트 설정 ==============================================
	switch(baudRate){
		case b1M:
			CANBT1= 0x00;
			CANBT2= 0x0c;
			CANBT3= 0x37;
			break;
		case b500K:
			CANBT1= 0x02;
			CANBT2= 0x0c;
			CANBT3= 0x37;			
			break;
		case b250K:
			CANBT1= 0x06;	// CAN보레이트 설정 
			CANBT2= 0x0c;	// bit timing: datasheet 264 (check table)
			CANBT3= 0x37;	// 250kbps, 16 MHz CPU Clock(0.250usec)
			break;
		case b200K:
			CANBT1= 0x08;
			CANBT2= 0x0c;
			CANBT3= 0x37;
			break;
		case b125K:
			CANBT1= 0x0E;
			CANBT2= 0x0c;
			CANBT3= 0x37;
			break;
		case b100K:
			CANBT1= 0x12;
			CANBT2= 0x0c;
			CANBT3= 0x37;													
			break;
	}
	
	for(i=0; i<15; i++)		// Reset all MObs
	{
		CANPAGE = (i<<4);	// MOBNB3~0
							// MOb Number Select(0~14)
		CANCDMOB = 0;		// ALL Disable MOb
		CANSTMOB = 0;		// Clear status
		CANIDT1 = 0;		// Clear ID
		CANIDT2 = 0;		// Clear ID
		CANIDT3 = 0;		// Clear ID
		CANIDT4 = 0;		// Clear ID
		CANIDM1 = 0;		// Clear mask
		CANIDM2 = 0;		// Clear mask
		CANIDM3 = 0;		// Clear mask
		CANIDM4 = 0;		// Clear mask

		for(j=0; j<8; j++)
			CANMSG = 0;		// CAN Data Message Register
							// Clear data
	}
							
							// Clear CAN interrupt registers
	CANGIE = 0;				// CAN General Interrupt Enable Register 
							// None Interrupts
	CANIE1 = 0;				// CAN Enable INT MOb Registers 1
							// None Interrupts on MObs
	CANIE2 = 0;				// CAN Enable INT MOb Registers 2
							// None Interrupts on MObs
	CANSIT1 = 0;			// CAN Status INT MOb Registers 1
							// None Interrupts on MObs
	CANSIT2 = 0;			// CAN Status INT MOb Registers 2
							// None Interrupts on MObs

	CANGCON = (1<<TTC );	// TTC mode *******************************************
	
	CANGCON |= (1<<ENASTB);	// CAN General Control Register 
							// Enable Mode (11 Recessive Bits has Been read)
							// Start CAN interface

	while (!(CANGSTA & (1<<ENFG))); // CAN General Status Register (Enable Flag)
									// Wait until module ready
}

//***************************************************************
// CAN 메시지 전송 (폴링 방식)	
// 처리 순서 
// 1. MOb 선택 
// 2. MOb 상태 클리어
// 3. CANCDMOB 클리어 (모드 설정, 자동 응답, IDE, DLC)
// 4. ID 설정 
// 5. IDE 설정 
// 6. DLC 설정 
// 7. RTR 설정 
// 8. 메시지 데이터 대입 
// 9. 메시지 송신 	
// 변수 
//		obj; MOb 번호 
//		msg; 메시지 구조체 
//		rtr; RTR r결정(0; 데이터 프레임, 1; 리모트 프레임)	
//***************************************************************
char can_tx (unsigned char obj, struct MOb *msg)//, char rtr)	// CAN transmission
{
	char send_result = _SEND_FAIL;
	unsigned char i;	
	unsigned long can_id= msg->id;
	
	long timeout=100000;
								// Enable MOb1, auto increment index, start with index = 0
	CANPAGE = (obj<<4);			// CAN Page MOb Register
								// MOb Number Select
	CANSTMOB = 0x00;
	CANCDMOB = 0x00;

	if(msg->ide== 0x00)	// standard
	{
		CANIDT1= (unsigned char)(can_id>>3);
		CANIDT2= (unsigned char)(can_id<<5);

		CANCDMOB &= ~0x10;		// Set IDE bit 2.0A 11bits
		usart1_transmit_string("\rstandard\n");
	}
	else	// extended
	{
		CANIDT1= (unsigned char)(can_id>>21);
		CANIDT2= (unsigned char)(can_id>>13);
		CANIDT3= (unsigned char)(can_id>>5);
		CANIDT4= (unsigned char)(can_id<<3);

		CANCDMOB |= 0x10;		// Set IDE bit 2.0B 29bits
		usart1_transmit_string("\rExtended\n");
	}

	CANCDMOB |= (msg->dlc<<DLC0);	// set data length

	CANIDT4 |= (msg->rtr & 0x04);     // RTRTAG 설정;

	CANIDT4 &= ~0x02;		   // RB1TAG=0;
	CANIDT4 &= ~0x01;		   // RB0TAG=1;

	//put data in mailbox
	for(i=0; i<msg->dlc; i++)
		CANMSG = msg->data[i];	// full message 

	//enable transmission		
	CANCDMOB |= (1<<CONMOB0);

	while ((!(CANSTMOB & (1<<TXOK))) && (timeout--));	// check tx ok
	send_result= _SEND_OK;

	// monitoring with serial com
	if(timeout>0)	 usart1_transmit_string("\rTXOK\n");
	else usart1_transmit_string("\rTX error (Timeout)\n");

	//reset flag
	CANSTMOB &= ~(1<<TXOK);

	return(send_result);
}


//***************************************************************
// CAN 메시지 수신 (폴링 방식)
// 1. 수신 MOb 선택 
// 2. 수신 완료 확인 
// 3. 표준 혹은 확장 버전 확인 
// 4. ID 수신 처리 
// 5. IDE 수신 처리 
// 6. DLC 수신 처리 
// 7. Data 수신 처리 
//***************************************************************
char can_rx(unsigned char obj, struct MOb *msg)		
{
	char rece_result = _RECE_FAIL;
	unsigned char i;	
	unsigned long can_id= 0;
	
	CANPAGE = (obj<<4);				// 수신 MOb 선택 

	usart1_transmit_string("\rRX MOb #");
	usart1_transmit(obj+0x30);
	usart1_transmit_string("\r\n");

	// 수신 완료될 때까지 대기함 
	while(!(CANSTMOB&(1<<RXOK)));
	// 수신이 완료되면 
	// CANIDT, CANCDMOB, CANMSG에 수신 메시지가 저장됨 
	// get CANIDT and CANCDMOB and CANMSg
	usart1_transmit_string("\rRXOK\n");
	rece_result = _RECE_OK;

	// 표준 혹은 확장 포맷에 맞추어 ID, IDE 결정 
	if((CANCDMOB & 0x10) == 0x00){			// IDE standard ?
		msg->ide= STD;
		can_id  = ((unsigned long)CANIDT1)<<8;
		can_id |= ((unsigned long)CANIDT2);
		can_id>>=5;
		usart1_transmit_string("\rRx Standard\n");
	}
	else{
		msg->ide= EXT;
		can_id  = ((unsigned long)CANIDT1)<<24;
		can_id |= ((unsigned long)CANIDT2)<<16;
		can_id |= ((unsigned long)CANIDT3)<<8;
		can_id |= ((unsigned long)CANIDT4);
		can_id>>=3;
		usart1_transmit_string("\rRx Extended\n");
	}
	msg->id= can_id;			// 구조체 변수로 CANID 대입 

	msg->rtr= CANIDT4 & 0x04;   

	msg->dlc= CANCDMOB & 0x0f;	// 수신 메시지 길이 구조체 변수에 대입 

	// get data
	for(i=0; i<(CANCDMOB&0xf); i++){
		msg->data[i] = CANMSG;	// 메시지 데이터 배열에 저장 
	}

	// rx init 
	CANSTMOB = 0x00;			// 상태 초기 화 

	// enable reception mode and ide set
	CANCDMOB |= (1<<CONMOB1); 	// 수신 IDE 유지하고 수신 모드 설정

	// reset flag
	CANSTMOB &= ~(1<<RXOK);		// 수신대기 

	return(rece_result);
}

//****************************************************************/
// CAN 수신 MOb 설정 
// 변수:
//		obj; MOb 번호 
//		id; CAN ID
//		ide; 메시지 포맷(0:2.0A, 1:2.0B)
//		dlc; 메시지 데이터 길이 (최대 8bytes)
//		idmask; CAN ID 수신 마스크 
//		rtrIdemask; RTR 비트와 IDE 마스크 			
//****************************************************************/
void can_rx_set(char obj, unsigned long id, char ide, unsigned char dlc, unsigned long idmask, unsigned char rtrIdemask)
{
	CANPAGE = obj<<4;		// set MOb number

	CANSTMOB = 0x00;		// clear status

	if(ide== STD)			// standard
	{
		CANIDT1= (unsigned char)(id>>3);
		CANIDT2= (unsigned char)(id<<5);

		CANIDM1= (unsigned char)(idmask>>3);
		CANIDM2= (unsigned char)(idmask<<5);
		CANIDM4=0;

		CANCDMOB &= ~0x10;	// clear IDE =0, standard 11 bits

		usart1_transmit_string("\rRx Standard Set\n");
	}
	else					// extended
	{
		CANIDT1= (unsigned char)(id>>21);
		CANIDT2= (unsigned char)(id>>13);
		CANIDT3= (unsigned char)(id>>5);
		CANIDT4= (unsigned char)(id<<3);

		CANIDM1= (unsigned char)(idmask>>21);
		CANIDM2= (unsigned char)(idmask>>13);
		CANIDM3= (unsigned char)(idmask>>5);
		CANIDM4= (unsigned char)(idmask<<3);

		CANCDMOB |= 0x10;	// set IDE =1, extended 29 bits

		usart1_transmit_string("\rRx Extended Set\n");
	}
	CANCDMOB |= (dlc & 0x0f);		// set data length

	CANIDM4 |= (unsigned char)(rtrIdemask & 0x07);
//	CANIDM4 &= ~0x04;		// RTRMSK= 1/0 enable comparison (Data receive)
//	CANIDM4 &= ~0x02;		// recommended
//	CANIDM4 &= ~0x01;		// IDEMSK= 1/0 enable comparison (IDE receive)

	CANCDMOB |= 0x80;		// receive enable 
}

//****************************************************************/
// CAN Interrupt 수신 MOb 설정 
// 변수:
//		obj; MOb 번호 
//		id; CAN ID
//		ide; 메시지 포맷(0:2.0A, 1:2.0B)
//		dlc; 메시지 데이터 길이 (최대 8bytes)
//		idmask; CAN ID 수신 마스크 
//		rtrIdemask; RTR 비트와 IDE 마스크 			
// 순서:
//  1. MOb 선택  
//  2. ID 설정 
//  3. 포맷 설정 
//  4. 마스크 설정 
//  5. DLC 설정 
//  6. 인터럽트 인에이블 (interrupt)
//  7. 수신 모드 설정 
//****************************************************************/
void can_int_rx_set(char obj, unsigned long id, char rplvIde, unsigned char dlc, unsigned long idmask, unsigned char rtrIdemask)
{
	CANPAGE = obj<<4;		// set MOb number

	CANSTMOB = 0x00;		// clear status

	if(rplvIde & 0x02)	
		CANCDMOB |= 0x20;			// RPLV set, 자동 응답 모드 설정 
	else
		CANCDMOB &= ~0x20;			// RPLV clear

	if(( rplvIde & 0x01) == STD)			// standard
	{
		CANIDT1= (unsigned char)(id>>3);
		CANIDT2= (unsigned char)(id<<5);

		CANIDM1= (unsigned char)(idmask>>3);
		CANIDM2= (unsigned char)(idmask<<5);
		CANIDM4=0;

		CANCDMOB &= ~0x10;	// clear IDE =0, standard 11 bits

		usart1_transmit_string("\rRx Standard Set\n");
	}
	else					// extended
	{
		CANIDT1= (unsigned char)(id>>21);
		CANIDT2= (unsigned char)(id>>13);
		CANIDT3= (unsigned char)(id>>5);
		CANIDT4= (unsigned char)(id<<3);

		CANIDM1= (unsigned char)(idmask>>21);
		CANIDM2= (unsigned char)(idmask>>13);
		CANIDM3= (unsigned char)(idmask>>5);
		CANIDM4= (unsigned char)(idmask<<3);

		CANCDMOB |= 0x10;	// set IDE =1, extended 29 bits

		usart1_transmit_string("\rRx Extended Set\n");
	}
	CANCDMOB |= (dlc & 0x0f);		// set data length

	CANIDM4 |= (unsigned char)(rtrIdemask & 0x07);
//	CANIDM4 &= ~0x04;		// RTRMSK= 1/0 enable comparison (Data receive)
//	CANIDM4 &= ~0x02;		// recommended
//	CANIDM4 &= ~0x01;		// IDEMSK= 1/0 enable comparison (IDE receive)


//  인터럽트 인에이블(인터럽트 설정)
	CANGIE |= 0xa0; 		// Enable all interrupt and Enable Rx interrupt

	if(obj<8) 
		CANIE2 = (1<<obj);		// 해당 MOb의 인터럽트를 인에이블 시킴 
	else        
		CANIE1 = (1<<(obj-8));	// 

	CANCDMOB |= 0x80;			// 수신 인에이블 
	sei();
}
