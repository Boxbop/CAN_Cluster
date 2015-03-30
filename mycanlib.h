
//CAN 관련 매크로  ======================================
#define STD		0x00
#define EXT		0x01

#define _SEND_FAIL	0
#define _SEND_OK	1

#define _RECE_FAIL	0
#define _RECE_OK	1

// CAN 보레이트(baud rate)===============================
#define b1M		1
#define b500k 	2
#define b250k	3
#define b200k	4
#define	b125k	5
#define b100k	6

//MOb 구조체 선언  ======================================
struct MOb
{
	unsigned long id;
	unsigned char rtr;
	unsigned char ide;
	unsigned char dlc;
	unsigned char data[8];
};

void can_init(char baudRate);
char can_tx(unsigned char obj, struct MOb *msg, char rtr);	// CAN transmission
char can_rx(unsigned char obj, struct MOb *msg);		
void can_rx_set(char obj, unsigned long id, char ide, unsigned char dlc, unsigned long idmask, unsigned char rtrIdemask);
void can_int_rx_set(char obj, unsigned long id, char ide, unsigned char dlc, unsigned long idmask, unsigned char rtrIdemask);
