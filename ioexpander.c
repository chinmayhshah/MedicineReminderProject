/*********************************************************************
Start Date : 4/03/2016
Version 1.3
code by :chinmay.shah@colorado.edu
Code : To implement to perform different Functions of I/O Expander(PCF8574)

Following functions have been Implemented and described

void IOWrite(unsigned char byte)
unsigned char IORead()
void IOSetInput(unsigned char bitposition )
void IOSetOutput(unsigned char bitposition )
unsigned char IOReadBit(unsigned char bitposition)
void IOMenu()

*********************************************************************/









#include "ioexpander.h"
#include "I2C.h"
#include <stdio.h>

#define DEBUG
/**************************************************
 Send a command to IO Expander
--Start -- WRITE (IOEXP_ADDR) --WRITE BYTE --STOP

***************************************************/
void IOWrite(unsigned char byte)
{
    I2CStart();//Start
    I2CSend(IOEXP_ADDRWR);//Send Addr and Start command to IO Expander
    I2CSend(byte);//Send data to IO Expander
    I2CStop();//Stop
}



/****************************************************
 Read a all Pins  from IO Expander
--Start -- WRITE (IOEXP_ADDRRD) --READ BYTE --STOP

*****************************************************/

unsigned char IORead()
{
    unsigned char byte;
    I2CStart();//Start
    I2CSend(IOEXP_ADDRRD);//Send Addr and Read command to IO Expander
    byte = I2CRecieve();//Receive data from IO Expander
    I2CStop();//Stop
    return byte ;
}



/****************************************************
 Read a bit from IO Expander
--Start -- WRITE (IOEXP_ADDRRD) --READ BYTE --STOP
Pre :Input bit position to check status
Post:Return Status of pin
*****************************************************/

unsigned char IOReadBit(unsigned char bitposition)
{
    unsigned char byte;
    byte=IORead();

    if (byte & (1<<bitposition))//check for particular bit position
    {
        return 1;
    }
    else
    {
        return 0;
    }

    #ifdef DEBUGALL
    printf("\n\r rx  value \n\r%02x",byte);
    #endif // DEBUG
}


/********************************************************
Set a particular pin as Input
--Start -- WRITE (IOEXP_ADDRRD) --WRITE PIN(HIGH) --STOP

Pre - Input bit /pin position to be set as input pin
*********************************************************/


void IOSetInput(unsigned char bitposition )
{
    unsigned char sendCommand;
    sendCommand = IORead();
    I2CStart();//Start
    I2CSend(IOEXP_ADDRWR);//Send Addr and Read command to IO Expander
    sendCommand |= (1 << bitposition );
    #ifdef DEBUGALL
    printf("\n\rsend value \n\r%02x",sendCommand);
    #endif // DEBUG
    I2CSend(sendCommand);
    I2CStop();//Stop
}


/*********************************************************
Set a particular pin as Output
--Start -- WRITE (IOEXP_ADDRRD) --WRITE PIN(HIGH) --STOP

Pre - Input bit /pin position to be set as output  pin
**********************************************************/

void IOSetOutput(unsigned char bitposition )
{
    unsigned char sendCommand;
    sendCommand = IORead();
    I2CStart();//Start
    I2CSend(IOEXP_ADDRWR);//Send Addr and Read command to IO Expander
    sendCommand &= ~(1 << bitposition );
    #ifdef DEBUGALL
    printf("\n\rsend value \n\r%02x",sendCommand);
    #endif // DEBUG
    I2CSend(sendCommand);
    I2CStop();//Stop
}


/*************************************************************

Menu for IO Expander

**************************************************************/


void IOMenu ()
{


    printf_tiny("\n\r IO LIST \r\n");
    printf_tiny("\n\r'i' -Set PIN Input \r\n");
    printf_tiny("\n\r'o' -Set PIN Output \r\n");
    printf_tiny("\n\r's' -Check Status for particular pin \r\n");
    printf_tiny("\n\r'r' -Reset BCD Counter  \r\n");
    printf_tiny("\n\r'?' -IO Menu \r\n");
    printf_tiny("\n\r'x' -Main Menu \r\n");

}
