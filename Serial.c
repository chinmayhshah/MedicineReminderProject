/********************************************
Coded by :Chinmay Shah
Version : V 1.0
Functions to Read and Write using RS232

***********************************************/


#include <at89c51ed2.h>
#include "timers.h"


/*******************************************
Function
1)Put char to put value on UART in SBUF
2) Clear Flag TI to start Transfer
3) Wait before Transmitting a new Data
********************************************/
void putchar (char c)
{
    //avoidWDTReset();
    while (!TI);
	//while ((SCON & 0x02) == 0); // wait for TX ready, spin on TI
	SBUF = c;  	// load serial port with transmit value
	TI = 0;  	// clear TI flag
	//avoidWDTReset();
}




/*******************************************
Function
1)GET a char from UART in SBUF
2) Clear Flag RI to start Transfer
3) Wait before filling Buffer again
********************************************/
char getchar ()
{
    //avoidWDTReset();
    while (!RI);                // compare asm code generated for these three lines
                                //while ((SCON & 0x01) == 0);  // wait for character to be received, spin on RI
	RI = 0;			            // clear RI flag
	return SBUF;  	            // return character from SBUF
	//avoidWDTReset();
}


