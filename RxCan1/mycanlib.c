#include <avr/io.h>
#include <avr/interrupt.h>

#include "myCANLib.h"
//#include "lcdControl.h"

//***************************************************************
// CAN �ʱ�ȭ 	
// 1. CAN ����� ���� 
// 2. ������Ʈ ���� 
// 3. MOb ���/���� �ʱ�ȭ 
// 4. MOb IDT, Mask �ʱ�ȭ 
// 5. CAN General ���ͷ�Ʈ Ŭ����(CAN ���ͷ�Ʈ ������� ����) 
// 6. CAN MOb ���ͷ�Ʈ Ŭ����(CAN MOb ���ͷ�Ʈ ������� ����)
// 7. CAN ����� �ο��̺� ��� ���� 
// 8. CAN ����� ���� Ȯ�� �� �ʱ�ȭ �Ϸ�  									
//***************************************************************
void can_init (char baudRate)	// CAN�ʱ�ȭ 
{
	unsigned char i, j;
	
	CANGCON |= (1<<SWRES);	// CAN ����� ����
							// CAN General Control Register
  							
	//������Ʈ ���� ==============================================
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
			CANBT1= 0x06;	// CAN������Ʈ ���� 
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
// CAN �޽��� ���� (���� ���)	
// ó�� ���� 
// 1. MOb ���� 
// 2. MOb ���� Ŭ����
// 3. CANCDMOB Ŭ���� (��� ����, �ڵ� ����, IDE, DLC)
// 4. ID ���� 
// 5. IDE ���� 
// 6. DLC ���� 
// 7. RTR ���� 
// 8. �޽��� ������ ���� 
// 9. �޽��� �۽� 	
// ���� 
//		obj; MOb ��ȣ 
//		msg; �޽��� ����ü 
//		rtr; RTR r����(0; ������ ������, 1; ����Ʈ ������)	
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

	CANIDT4 |= (msg->rtr & 0x04);     // RTRTAG ����;

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
// CAN �޽��� ���� (���� ���)
// 1. ���� MOb ���� 
// 2. ���� �Ϸ� Ȯ�� 
// 3. ǥ�� Ȥ�� Ȯ�� ���� Ȯ�� 
// 4. ID ���� ó�� 
// 5. IDE ���� ó�� 
// 6. DLC ���� ó�� 
// 7. Data ���� ó�� 
//***************************************************************
char can_rx(unsigned char obj, struct MOb *msg)		
{
	char rece_result = _RECE_FAIL;
	unsigned char i;	
	unsigned long can_id= 0;
	
	CANPAGE = (obj<<4);				// ���� MOb ���� 

	usart1_transmit_string("\rRX MOb #");
	usart1_transmit(obj+0x30);
	usart1_transmit_string("\r\n");

	// ���� �Ϸ�� ������ ����� 
	while(!(CANSTMOB&(1<<RXOK)));
	// ������ �Ϸ�Ǹ� 
	// CANIDT, CANCDMOB, CANMSG�� ���� �޽����� ����� 
	// get CANIDT and CANCDMOB and CANMSg
	usart1_transmit_string("\rRXOK\n");
	rece_result = _RECE_OK;

	// ǥ�� Ȥ�� Ȯ�� ���˿� ���߾� ID, IDE ���� 
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
	msg->id= can_id;			// ����ü ������ CANID ���� 

	msg->rtr= CANIDT4 & 0x04;   

	msg->dlc= CANCDMOB & 0x0f;	// ���� �޽��� ���� ����ü ������ ���� 

	// get data
	for(i=0; i<(CANCDMOB&0xf); i++){
		msg->data[i] = CANMSG;	// �޽��� ������ �迭�� ���� 
	}

	// rx init 
	CANSTMOB = 0x00;			// ���� �ʱ� ȭ 

	// enable reception mode and ide set
	CANCDMOB |= (1<<CONMOB1); 	// ���� IDE �����ϰ� ���� ��� ����

	// reset flag
	CANSTMOB &= ~(1<<RXOK);		// ���Ŵ�� 

	return(rece_result);
}

//****************************************************************/
// CAN ���� MOb ���� 
// ����:
//		obj; MOb ��ȣ 
//		id; CAN ID
//		ide; �޽��� ����(0:2.0A, 1:2.0B)
//		dlc; �޽��� ������ ���� (�ִ� 8bytes)
//		idmask; CAN ID ���� ����ũ 
//		rtrIdemask; RTR ��Ʈ�� IDE ����ũ 			
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
// CAN Interrupt ���� MOb ���� 
// ����:
//		obj; MOb ��ȣ 
//		id; CAN ID
//		ide; �޽��� ����(0:2.0A, 1:2.0B)
//		dlc; �޽��� ������ ���� (�ִ� 8bytes)
//		idmask; CAN ID ���� ����ũ 
//		rtrIdemask; RTR ��Ʈ�� IDE ����ũ 			
// ����:
//  1. MOb ����  
//  2. ID ���� 
//  3. ���� ���� 
//  4. ����ũ ���� 
//  5. DLC ���� 
//  6. ���ͷ�Ʈ �ο��̺� (interrupt)
//  7. ���� ��� ���� 
//****************************************************************/
void can_int_rx_set(char obj, unsigned long id, char rplvIde, unsigned char dlc, unsigned long idmask, unsigned char rtrIdemask)
{
	CANPAGE = obj<<4;		// set MOb number

	CANSTMOB = 0x00;		// clear status

	if(rplvIde & 0x02)	
		CANCDMOB |= 0x20;			// RPLV set, �ڵ� ���� ��� ���� 
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


//  ���ͷ�Ʈ �ο��̺�(���ͷ�Ʈ ����)
	CANGIE |= 0xa0; 		// Enable all interrupt and Enable Rx interrupt

	if(obj<8) 
		CANIE2 = (1<<obj);		// �ش� MOb�� ���ͷ�Ʈ�� �ο��̺� ��Ŵ 
	else        
		CANIE1 = (1<<(obj-8));	// 

	CANCDMOB |= 0x80;			// ���� �ο��̺� 
	sei();
}
