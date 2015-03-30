#include <avr/io.h>
#include <stdio.h>

#include "mycanlib.h"

struct MOb msg1 = {	0x023,0,EXT,8,{0}}; //id,rtr,ide,dlc,data

void initPort()
{
	DDRC = 0xff;
	PORTC = 0xff;
	DDRG = 0xff;
	PORTG = 0x00;
}

void main()
{
	char strBuff[20]={0};
	char count;
	initPort();
	can_init(3); //b250k
	count =0;
	msg1.rtr=0;

	msg1.id=0;
	msg1.ide=STD;
	msg1.dlc=0;

//	unsigned char IdNumber;
//	unsigned char DataLength;

	while(1)
	{
		msg1.dlc=0;
		for(unsigned char id = 0; id < 3000; id++)
		{
			msg1.id=id;
			//for(unsigned char data = 0; data < 256; data++)
			//{//
				can_tx(7,&msg1,0);
			//}
		}

		msg1.dlc=1;
		for(unsigned char id = 0; id < 3000; id++)
		{
			msg1.id=id;
		  	  for(unsigned char data0 = 0; data0 < 256; data0++)
			  {
			  	msg1.data[0]=data0;
				can_tx(7,&msg1,0);
			  }
		}

		msg1.dlc=2;
		for(unsigned char id = 0; id < 3000; id++)
		{
			msg1.id=id;
			for(unsigned char data1 = 0; data1 < 256; data1++)
			{
			msg1.data[1]=data1;
		  	  for(unsigned char data0 = 0; data0 < 256; data0++)
			  {
			  	msg1.data[0]=data0;
				can_tx(7,&msg1,0);
			  }
			}
		}

		msg1.dlc=3;
		for(unsigned char id = 0; id < 3000; id++)
		{
			msg1.id=id;
		 	for(unsigned char data2 = 0; data2 < 256; data2++)
		  	{
		    msg1.data[2]=data2;
				for(unsigned char data1 = 0; data1 < 256; data1++)
				{
				  msg1.data[1]=data1;
		  	  	  for(unsigned char data0 = 0; data0 < 256; data0++)
			 	  {
			 	 	msg1.data[0]=data0;
					can_tx(7,&msg1,0);
				  }	
				}
			}
		}

		msg1.dlc=4;
		for(unsigned char id = 0; id < 3000; id++)
		{
			msg1.id=id;

			for(unsigned char data3 = 0; data3 < 256; data3++)
			{
			msg1.data[3]=data3;
		 	for(unsigned char data2 = 0; data2 < 256; data2++)
		  	{
		    msg1.data[2]=data2;
				for(unsigned char data1 = 0; data1 < 256; data1++)
				{
				  msg1.data[1]=data1;
		  	  	  for(unsigned char data0 = 0; data0 < 256; data0++)
			 	  {
			 	 	msg1.data[0]=data0;
					can_tx(7,&msg1,0);
				  }	
				}
			}
			}

		}





	}

}



/*	while(1)
	{

	}
	while(1)
	{
		count++;
		if(count==10) count =0;
		msg1.data[4] = count+0x30;
		can_tx(7,&msg1,0);

		//sprintf(strBuff,"TxData=%d",msg1.data[4]);
	}
*/

